#include "TargetingComponent.h"
#include "../../Structs/CommonUtility/FLogger.h"
#include "GameFramework/PlayerController.h"
#include "../../Characters/Player/PraisePlayerCharacter.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../Components/Actor/CharStats/BotStatsComponent.h"
#include "../../Characters/BasePraiseCharacter.h"

// Sets default values for this component's properties
UTargetingComponent::UTargetingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoRegister = true;
	bAutoActivate = true;
	bAutoTarget = false;
	bCheckFOV = true;
	bCheckCol = false;
	bCheckDistance = true;
	bIgnoreSprint = false;
	bIsVisionBlocked = false;

	TraceRange = 1500.f;
	TraceWidth = 300.f;
	TargetLockRotSpeed = 40.f;
	MaxVisionBlockSeconds = 3.f;
	VisionBlockSeconds = 0.f;

	// ...
}

//
//
//// Called when the game starts
void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
	// ...
}

bool UTargetingComponent::CanRotate()
{
	if (!GetOwner()) return false;

	ABasePraiseCharacter* CompOwner = GetCompOwner<ABasePraiseCharacter>();

	if (!CompOwner) return false;

	if (!CompOwner->GetCharStats()) return false;

	if (CompOwner->GetCharStats()->IsDead()) return false;

	if (!CurrentTarget) return false;

	if (!CurrentTarget->GetCharStats()) return false;

	if (CurrentTarget->GetCharStats()->IsDead()) return false;

	FVector ToTarget = CurrentTarget->GetActorLocation() - CompOwner->GetActorLocation();

	if (/*!bIgnoreSprint && */ CompOwner->IsRunning()) return false;

	if (CompOwner->IsBeingDamaged()) return false;

	if (CompOwner->IsAttacking() /*&& CompOwner->GetCurrentPose() != EArmedPoses::RIFLE*/) return false; 

	if (CompOwner->IsAiming()) return false;

	if (CompOwner->IsRolling()) return false;

	if (CompOwner->IsEvading()) return false;

	if (!CompOwner->GetMovementComponent()) return false;

	if (CompOwner->GetMovementComponent()->IsFalling()) return false;

	return true;
}

bool UTargetingComponent::CanTarget()
{
	
	if (!GetCompOwner<ABasePraiseCharacter>()) return false;

	if (!GetCompOwner<ABasePraiseCharacter>()->GetCharStats()) return false;
	
	if (bIsVisionBlocked) return false;
	
	return !GetCompOwner<ABasePraiseCharacter>()->GetCharStats()->IsDead();
}

bool UTargetingComponent::HasValidTarget()
{

	if (!CurrentTarget) return false;

	if (!IsTargetLocked()) return false;

	if (!CurrentTarget->GetCharStats()) return false;
	
	return !CurrentTarget->GetCharStats()->IsDead();
}

bool UTargetingComponent::ShouldDisable()
{
	return !HasValidTarget() || !CanTarget() || (bCheckDistance && !IsInTargetingRange());
}

bool UTargetingComponent::TryLockTarget(bool bTrackDistance)
{
	ABasePraiseCharacter* Targeter = GetCompOwner<ABasePraiseCharacter>();

	if (!Targeter) return false;

	ABasePraiseCharacter* Target = CurrentTarget = nullptr;
	bIsTargetLocked = false;
	bCheckDistance = bTrackDistance;
	VisionBlockSeconds = 0.f;
	bIsVisionBlocked = false;
	TArray<ABasePraiseCharacter*> Targets;
	switch (bAutoTarget) {
		case(true):
		
			Targets = GetTargetsArroundActor(GetOwner());

			if (Targets.Num() == 0) return false;

			Target = GetClosestTarget(Targets, bCheckFOV, bCheckCol);

			if (!Target) return false;

			CurrentTarget = Target;

			Enable(true);

			break;
		case(false):
			FVector Loc;
			FRotator Rot;
		
			Targeter->GetPlayerViewPoint(Loc, Rot, Targeter->IsA<APraisePlayerCharacter>());
		
			if (TryBoxCast(Loc,Rot.Vector()))
				Enable(true);

			else return false;

			break;
	}

	return CurrentTarget != nullptr;
}

bool UTargetingComponent::ForceLockTarget(AActor* Target, bool bTrackDistance)
{
	if (!Target->IsA<ABasePraiseCharacter>()) return false;

	bCheckDistance = bTrackDistance;

	CurrentTarget = Cast<ABasePraiseCharacter>(Target);

	Enable(true);

	return true;
}

ABasePraiseCharacter* UTargetingComponent::GetClosestFromSweep(TArray<FHitResult>& OutHits, bool bCheckingFOV, bool bCheckingCol)
{
	ABasePraiseCharacter* Closest = nullptr;
	FVector Distance = FVector::ZeroVector;
	for (int i = 0; i < OutHits.Num(); i++) 
	{
		if (OutHits[i].Actor == nullptr) continue;
		
		if (!OutHits[i].Actor->IsA<ABasePraiseCharacter>()) continue;

		ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(OutHits[i].Actor);

		if (bCheckingFOV && !IsInsideFOV(Char, bCheckingCol)) continue;

		FVector distance = Char->GetActorLocation() - GetOwner()->GetActorLocation();

		if ((Distance == FVector::ZeroVector) || (distance.Size() < Distance.Size())) 
		{
			Closest = Char;
			Distance = distance;
		}
	}

	return Closest;
}

ABasePraiseCharacter* UTargetingComponent::GetClosestTarget(TArray<ABasePraiseCharacter*> OutHits, bool bCheckingFOV, bool bCheckingCol)
{
	ABasePraiseCharacter* Closest = nullptr;
	FVector Distance = FVector::ZeroVector;
	for (int i = 0; i < OutHits.Num(); i++) 
	{
		if (!OutHits[i]->IsA<ABasePraiseCharacter>()) continue;

		ABasePraiseCharacter* Char = OutHits[i];

		FVector distance = Char->GetActorLocation() - GetOwner()->GetActorLocation();

		if ((Distance == FVector::ZeroVector) || (distance.Size() < Distance.Size())) {
			Closest = Char;
			Distance = distance;
		}
	}

	return Closest;
}

template<typename T>
T* UTargetingComponent::GetClosestTarget(AActor* Origin, bool bIsDistanceToTarget, bool bCheckingFOV, bool bCheckingCol, float Radius, ECharVector Dir, float AlignmentToleranceDegs, ECollisionChannel Channel, TArray<AActor*> Ignored)
{
	if (bIsDistanceToTarget && !CurrentTarget) return nullptr;

	T* Closest = nullptr;

	TArray<FHitResult> OutHits;
	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius <= 0 ? TraceRange : Radius);
	FCollisionQueryParams QueryParams;
	for (AActor* ignored : Ignored) {
		QueryParams.AddIgnoredActor(ignored);
	}

	if (GetWorld()->SweepMultiByChannel(OutHits, Origin->GetActorLocation(), Origin->GetActorLocation(), FQuat::Identity, Channel, Shape, QueryParams))
	{
		FVector Distance;
		for (int i = 0; i < OutHits.Num(); i++)
		{
			if (!OutHits[i].Actor->IsA<T>()) continue;

			FVector distance = bIsDistanceToTarget ? OutHits[i].Actor->GetActorLocation() - CurrentTarget->GetActorLocation() : OutHits[i].Actor->GetActorLocation() - Origin->GetActorLocation();

			if (Closest == nullptr)
			{
				if (Origin->IsA<ABasePraiseCharacter>())
					if (!IsTargetAligned(Cast<ABasePraiseCharacter>(Origin), OutHits[i].Actor->GetActorLocation(), Dir, AlignmentToleranceDegs))
						continue;

				if (bCheckingFOV)
				{
					if (!IsInsideFOV(OutHits[i].Actor.Get(), bCheckingCol))
						continue;
				}
				else
				{
					if(bCheckingCol && !GetCompOwner<ABasePraiseCharacter>()->CanSeeTarget(OutHits[i].Actor.Get(), false, false))
						continue;
				}

				Closest = Cast<T>(OutHits[i].Actor);
				Distance = distance;
				continue;
			}

			if (distance.Size() < Distance.Size())
			{
				if (Origin->IsA<ABasePraiseCharacter>())
					if (!IsTargetAligned(Cast<ABasePraiseCharacter>(Origin), OutHits[i].Actor->GetActorLocation(), Dir, AlignmentToleranceDegs))
						continue;

				if (bCheckingFOV)
				{
					if (!IsInsideFOV(OutHits[i].Actor.Get(), bCheckingCol))
						continue;
				}
				else
				{
					if (bCheckingCol && !GetCompOwner<ABasePraiseCharacter>()->CanSeeTarget(OutHits[i].Actor.Get(), false, false))
						continue;
				}

				Closest = Cast<T>(OutHits[i].Actor);
				Distance = distance;
			}
		}
	}

	return Closest;
}

bool UTargetingComponent::IsInsideFOV(AActor* Target, bool bCheckingCol)
{
	return GetOwner()->IsA<APraisePlayerCharacter>() ?
		GetCompOwner<APraisePlayerCharacter>()->IsTargetInCameraFOV(Target, bCheckingCol) :
		GetCompOwner<ABasePraiseCharacter>()->IsTargetInsideFOV(Target, bCheckingCol);
}

bool UTargetingComponent::IsInsideFOV(AActor* Origin, AActor* Target, bool bCheckingCol)
{
	if (!Origin->IsA<ABasePraiseCharacter>()) return true;

	return Origin->IsA<APraisePlayerCharacter>() ?
		Cast<APraisePlayerCharacter>(Origin)->IsTargetInCameraFOV(Target, bCheckingCol) :
		Cast<ABasePraiseCharacter>(Origin)->IsTargetInsideFOV(Target, bCheckingCol);
}


bool UTargetingComponent::TryBoxCast(FVector Origin, FVector Direction)
{
	if (!GetOwner()) return false;

	TArray<FHitResult> OutHits;

	FVector BoxExtent = FVector(TraceWidth, TraceWidth, TraceWidth);

	bool bIsPlayer = GetOwner()->IsA<APraisePlayerCharacter>();
	
	FRotator BoxRot = bIsPlayer ? GetCompOwner<APraisePlayerCharacter>()->GetPlayerCamera()->GetForwardVector().Rotation() : GetCompOwner<ABasePraiseCharacter>()->GetViewRotation();
	FQuat Rot = BoxRot.Quaternion();

	FCollisionShape Box = FCollisionShape::MakeBox(BoxExtent);
	FCollisionQueryParams ColQueryParams;
	ColQueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->SweepMultiByChannel(OutHits, Origin, Origin + Direction * TraceRange, Rot, ECollisionChannel::ECC_Visibility, Box, ColQueryParams))
	{
		ABasePraiseCharacter* Hitten = GetClosestFromSweep(OutHits, /*bCheckFOV:*/ false, /*bCheckCol:*/ true);

		if (!Hitten) return false;

		CurrentTarget = Cast<ABasePraiseCharacter>(Hitten);

		return true;
	}

	else return false;
}

bool UTargetingComponent::IsInTargetingRange()
{

	if (!CurrentTarget) return false;
	
	if (!GetOwner()) return false;
	
	FVector Vector = CurrentTarget->GetActorLocation() - GetOwner()->GetActorLocation();
	
	return Vector.Size() <= TraceRange;
}

bool UTargetingComponent::TrySkipTarget(bool bSkipLeft)
{
	if (!CurrentTarget) return false;

	ECharVector Dir = bSkipLeft ? ECharVector::LEFT : ECharVector::RIGHT;
	
	ABasePraiseCharacter* NewTarget = GetClosestTarget<ABasePraiseCharacter>(GetOwner(), true, false, true, TraceRange, Dir, 0.f,
		ECollisionChannel::ECC_GameTraceChannel3, TArray<AActor*> { CurrentTarget, GetCompOwner<APraisePlayerCharacter>()});

	if (NewTarget && NewTarget != GetCompOwner<APraisePlayerCharacter>() && NewTarget != CurrentTarget) 
	{
		if (GetOwner()->IsA<APraisePlayerCharacter>())
		{
			CurrentTarget->EnableTargetWidget(false);
			NewTarget->EnableTargetWidget(true);
		}
			
		CurrentTarget = NewTarget;
		
		return true;
	}

	return false;
}

bool UTargetingComponent::IsTargetAligned(ABasePraiseCharacter* Origin, FVector TargetLoc, ECharVector Vector, float FwdAxisToleranceDegs)
{
	switch (Vector)
	{
		case(ECharVector::FWD):
			return Origin->IsTargetForward(TargetLoc, FwdAxisToleranceDegs);
		case(ECharVector::BWD):
			return Origin->IsTargetBackward(TargetLoc, FwdAxisToleranceDegs);
		case(ECharVector::LEFT):
			return Origin->IsTargetAside(TargetLoc, ECharVector::LEFT, FwdAxisToleranceDegs);
		case(ECharVector::RIGHT):
			return Origin->IsTargetAside(TargetLoc, ECharVector::RIGHT, FwdAxisToleranceDegs);
		case(ECharVector::NONE):
		default:
			break;
	}

	return false;
}

void UTargetingComponent::Enable(bool Value)
{	
	if (!CurrentTarget) return;

	bIsVisionBlocked = false;

	switch (Value)
	{
		case(true):

			bIsTargetLocked = true;
			
			if(!CurrentTarget->OnNotifyDead.IsAlreadyBound(GetCompOwner<ABasePraiseCharacter>(), &ABasePraiseCharacter::HandleCharDeadNotify))
				CurrentTarget->OnNotifyDead.AddDynamic(GetCompOwner<ABasePraiseCharacter>(), &ABasePraiseCharacter::HandleCharDeadNotify);

			SetComponentTickEnabled(true);

			break;

		case(false):
	
			bIsTargetLocked = false;

			GetCompOwner<ABasePraiseCharacter>()->DisableTargeting();

			if (CurrentTarget->OnNotifyDead.IsAlreadyBound(GetCompOwner<ABasePraiseCharacter>(), &ABasePraiseCharacter::HandleCharDeadNotify))
				CurrentTarget->OnNotifyDead.RemoveDynamic(GetCompOwner<ABasePraiseCharacter>(), &ABasePraiseCharacter::HandleCharDeadNotify);

			SetComponentTickEnabled(false);

			if(GetOwner()->IsA<ABasePraiseCharacter>())
				CurrentTarget->EnableTargetWidget(false);

			CurrentTarget = nullptr;

			break;
		}
}

TArray<ABasePraiseCharacter*> UTargetingComponent::GetTargetsArroundActor(AActor* Origin)
{
	TArray<ABasePraiseCharacter*>Targets;

	FCollisionShape Shape = FCollisionShape::MakeSphere(TraceRange);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	TArray<FHitResult> OutHits;
	if (GetWorld()->SweepMultiByChannel(OutHits, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel3, Shape, QueryParams)) {
		for (int i = 0; i < OutHits.Num(); i++) {
			if (OutHits[i].Actor->IsA<ABasePraiseCharacter>()) {
				if (!Cast<ABasePraiseCharacter>(OutHits[i].Actor)->GetCharStats()->IsDead()) {
					if (bOnlyEnemyTargets)
					{
						if (!OutHits[i].Actor->Implements<UFactioneable>()) continue;

						if (GetCompOwner<ABasePraiseCharacter>()->IsEnemyTarget(Cast<IFactioneable>(OutHits[i].Actor)))
							Targets.Add(Cast<ABasePraiseCharacter>(OutHits[i].Actor));
					}

					else Targets.Add(Cast<ABasePraiseCharacter>(OutHits[i].Actor));
				}
			}
		}
	}

	return Targets;
}

// Called every frame
void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldDisable()) { Enable(false); return; }
	
	if (CanRotate())
	{
		GetCompOwner<ABasePraiseCharacter>()->GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCompOwner<ABasePraiseCharacter>()->RotateTo(DeltaTime, CurrentTarget->GetActorLocation());
	}
		
	else
	{
		if(!GetCompOwner<ABasePraiseCharacter>()->IsAiming() && GetCompOwner<ABasePraiseCharacter>()->GetCurrentAttackType() != EAttackType::SECONDARY_ATTACK)
			GetCompOwner<ABasePraiseCharacter>()->GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	if (!GetCompOwner<ABasePraiseCharacter>()->HasObstacleInBetween(CurrentTarget))
	{
		bIsVisionBlocked = false;
		VisionBlockSeconds = 0.f;
	}

	else VisionBlockSeconds += DeltaTime;

	if (VisionBlockSeconds >= MaxVisionBlockSeconds)
		bIsVisionBlocked = true;
}

void UTargetingComponent::ClearTarget()
{
	Enable(false);
}