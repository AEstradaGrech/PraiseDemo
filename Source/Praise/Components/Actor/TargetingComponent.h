#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../Enums/CommonUtility/ECharVector.h"
#include "TargetingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingComponent();
	FORCEINLINE class ABasePraiseCharacter* GetCurrentTarget() const { return CurrentTarget; }
	FORCEINLINE bool IsAutoTarget() const { return bAutoTarget; }
	FORCEINLINE bool IsTargetLocked() const { return CurrentTarget && bIsTargetLocked; }
	FORCEINLINE bool IsCheckingDistance() const { return CurrentTarget && bCheckDistance; }
	FORCEINLINE bool IsCheckingFOV() const { return bCheckFOV; }
	FORCEINLINE bool IsIgnoringSprint() const { return bIgnoreSprint; }
	FORCEINLINE float GetLockRotationSpeed() const { return TargetLockRotSpeed; }
	FORCEINLINE float GetTargetingDistance() const { return TraceRange; }
	FORCEINLINE bool IsOnlyEnemyTargets() const { return bOnlyEnemyTargets; }

	FORCEINLINE void SetIsAutoTarget(bool bValue) { bAutoTarget = bValue; }
	FORCEINLINE void SetCheckDistance(bool bCheck) { bCheckDistance = bCheck; }
	FORCEINLINE void SetIgnoreSprint(bool bIgnore) { bIgnoreSprint = bIgnore; }
	FORCEINLINE void SetCheckFOV(bool bCheck) { bCheckFOV = bCheck; }
	FORCEINLINE void SetCheckCollision(bool bCheck) { bCheckCol = bCheck; }
	FORCEINLINE void SetOnlyEnemyTargets(bool bValue) { bOnlyEnemyTargets = bValue; }
	FORCEINLINE void SetTraceRange(float Range) { TraceRange = Range; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	bool CanRotate();
	bool CanTarget();
	bool HasValidTarget();
	bool ShouldDisable();
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		bool bIgnoreSprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		bool bAutoTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		bool bCheckDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		bool bCheckFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		bool bCheckCol;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TargetingSystem, meta = (AllowPrivateAccess = "true"))
		bool bIsTargetLocked;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		bool bOnlyEnemyTargets;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TargetingSystem, meta = (AllowPrivateAccess = "true"))
		class ABasePraiseCharacter* CurrentTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		float TraceRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		float TraceWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		float TargetLockRotSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetingSystem, meta = (AllowPrivateAccess = "true"))
		float MaxVisionBlockSeconds;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TargetingSystem, meta = (AllowPrivateAccess = "true"))
		float VisionBlockSeconds;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TargetingSystem, meta = (AllowPrivateAccess = "true"))
		bool bIsVisionBlocked;
	
	bool IsInsideFOV(AActor* Target, bool bCheckCol = false);
	bool IsInsideFOV(AActor* Origin, AActor* Target, bool bCheckCol = false);
	bool TryBoxCast(FVector Origin, FVector Direction);
	bool IsInTargetingRange();

	
	bool IsTargetAligned(class ABasePraiseCharacter* Origin, FVector TargetLoc, ECharVector Vector, float FwdVectorToleranceDegs = 45.f);
	
	void Enable(bool Value);
	TArray<class ABasePraiseCharacter*> GetTargetsArroundActor(AActor* Origin);

public:

	bool TrySkipTarget(bool bSkipLeft);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	template<typename T>
	T* GetCompOwner() const { return Cast<T>(GetOwner()); }

	void ClearTarget();
	bool TryLockTarget(bool bTrackDistance = true);
	UFUNCTION()
	bool ForceLockTarget(AActor* Target, bool bTrackDistance = true);
	class ABasePraiseCharacter* GetClosestFromSweep(TArray<FHitResult>& OutHits, bool bCheckFOV = false, bool bCheckCol = false);
	class ABasePraiseCharacter* GetClosestTarget(TArray<class ABasePraiseCharacter*> OutHits, bool bCheckFOV = false, bool bCheckCol = false);

	template<typename T>
	T* GetClosestTarget(AActor* Origin, bool bIsDistanceToTarget, bool bCheckFOV = false, bool bCheckCol = false, float Radius = 0.f,
		ECharVector Dir = ECharVector::NONE, float AlignmentToleranceDegs = 45.f, ECollisionChannel Channel = ECollisionChannel::ECC_Visibility, TArray<AActor*> Ignored = TArray<AActor*>());
};

