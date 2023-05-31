// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile() : Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shell Mesh"));
	ShellCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Shell Collider"));

	RootComponent = ShellCollider;
	ShellMesh->AttachTo(RootComponent);

	SetDefaults();
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetDefaults();

	GetWorld()->GetTimerManager().ClearTimer(BoomTimerHandle);
	
}

void AProjectile::SetDefaults()
{
	PrimaryActorTick.bCanEverTick = true;
	ShellCollider->SetSimulatePhysics(true);	
}

void AProjectile::Boom()
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BoomSFX, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BoomFX, GetActorLocation());
}

void AProjectile::HandleRetardedBoom()
{
	GetWorld()->GetTimerManager().SetTimer(BoomTimerHandle, this, &AProjectile::Boom, TimeToBoom);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
* SPHERE SWEEP EXAMPLE FROM : https://answers.unrealengine.com/questions/165523/on-component-begin-overlap-sweep-result-not-popula.html

 void AMyPawn::OnComponentBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
 {
     if (OtherActor && (OtherActor != this))
     {
         TArray<FHitResult> AllResults;

         // Get the location of this actor
         auto Start = GetActorLocation();
         // Get the location of the other component
         auto End = OtherComp->GetComponentLocation();
         // Use a slightly larger radius to ensure we find the same result
         auto CollisionRadius = FVector::Dist(Start, End) * 1.1f;

         // Now do a spherical sweep to find the overlap
         GetWorld()->SweepMultiByObjectType(
             AllResults,
             Start,
             End,
             FQuat::Identity,
             0,
             FCollisionShape::MakeSphere(CollisionRadius),
             FCollisionQueryParams::FCollisionQueryParams(false)
         );

         // Finally check which hit result is the one from this event
         for (auto HitResult : AllResults)
         {
             if (OtherComp->GetUniqueID() == HitResult.GetComponent()->GetUniqueID()) {
                 // A component with the same UniqueID means we found our overlap!

                 // Do your stuff here, using info from 'HitResult'
                 OnComponentBeginOverlapWithInfo(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, HitResult);

                 break;
             }
         }
     }
 }
*/
