// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Structs/CommonUtility/FLogger.h"
#include "Projectile.generated.h"

UCLASS()
class PRAISE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadonly)
		class UStaticMeshComponent* ShellMesh;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		class UCapsuleComponent* ShellCollider;
	
	float GetInitialSpeed() const { return InitialSpeed; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetDefaults();
	virtual void Boom();
	virtual void ApplyDamage(AActor* HittenActor, const FHitResult& HitResult) {};
	virtual void HandleRetardedBoom();

	UPROPERTY(EditAnywhere, BlueprintReadonly)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		float InitialSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		FVector2D CapsuleSize;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = FX)
    class UParticleSystem* BoomFX;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = FX)
	class USoundBase* BoomSFX;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
	bool bIsRetarded;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
	float TimeToBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Weapon)
	TSubclassOf<UDamageType> DamageType;

	FTimerHandle BoomTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
