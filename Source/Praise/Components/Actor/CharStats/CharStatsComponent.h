// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../../Enums/CommonUtility/ECurveType.h"
#include "../../../Enums/Characters/ECharStats.h"
#include "../../../Structs/Characters/FCharStats.h"
#include "../../../Praise.h"
#include "CharStatsHandler.h"
#include "CharStatsComponent.generated.h"


DECLARE_DELEGATE_OneParam(FOnSetDeadSignature, AActor*);
DECLARE_DELEGATE_OneParam(FOnSpeedModifiedSignature, ECurveType);
DECLARE_DELEGATE_TwoParams(FOnUpdateStatsSignature, ECharStats, float);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UCharStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharStatsComponent();
	FORCEINLINE float GetMaxHealth() const { return CharacterStats ? CharacterStats->GetMaxHealth() : DEFAULT_CHAR_HEALTH; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxStamina() const { return  CharacterStats ? CharacterStats->GetMaxStamina() : DEFAULT_CHAR_STAMINA; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE bool IsDead() const { return bIsDead; }
	FORCEINLINE ECharClass GetCharClass() const { return CharacterStats ? CharacterStats->Class : ECharClass::NONE; }
	FORCEINLINE ECharFaction GetCharFaction() const { return CharacterStats ? CharacterStats->Faction : ECharFaction::NONE; }
	FORCEINLINE UCharStatsHandler* GetStats() const { return CharacterStats; }
	FORCEINLINE float GetDefaultWalkSpeed() const { return DefaultWalkSpeed; }
	FORCEINLINE float GetMinWalkSpeed() const { return MinWalkSpeed; }
	FORCEINLINE float GetMaxWalkSpeed() const { return MaxWalkSpeed > DefaultWalkSpeed * GetMaxWalkSpeedMult() ? DefaultWalkSpeed * GetMaxWalkSpeedMult() : MaxWalkSpeed; }
	FORCEINLINE float GetSightRange() const { return SightRange; }
	FORCEINLINE float GetFieldOfView() const { return CharFOV; }
	FORCEINLINE float GetSpeedMult() const { return FMath::Clamp(WalkSpeedMult, 0.f, 3.f); }		
	FORCEINLINE float GetMaxWalkSpeedMult() const { return FMath::Clamp(MaxWalkSpeedMult, 2.f, 3.f); }  
	
	virtual float GetArmedSpeedMult() const { return ArmedSpeedMult; }
	float GetSprintSpeedMult() const { return SprintSpeedMult;}
	void UpdateStamina(float Value);
	virtual float GetResistance();
	float GetMaxResistance();
	float GetResistanceRatio();
	float GetHealthRatio() const;
	float GetStaminaRatio() const;
	virtual float GetWalkSpeed() const;
	FOnSetDeadSignature OnSetDead;
	FOnUpdateStatsSignature OnStatUpdate;
	FOnSpeedModifiedSignature OnSpeedChange;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		bool bIsDead;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
		float Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
		float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		float CharFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		float WalkSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		float MaxWalkSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float DefaultWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MinWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MaxWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float SightRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float SprintSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float ArmedSpeedMult;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCharStatsHandler* CharacterStats;

	
public:
	virtual float GetMainStatValue(ECharStats Stat);
	virtual void InitializeStatsObject(FCharStats* StatsStruct);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetDefaults();

	UFUNCTION()
		virtual void UpdateStatValue(ECharStats Stat, float Value);
	UFUNCTION()
		virtual float GetStatValue(ECharStats Stat);
	UFUNCTION()
		virtual void ManageHealth(float Value, AActor* Damager);
	UFUNCTION()
		virtual void UpdateHealth(float Value);
	UFUNCTION(BlueprintCallable)
		void SetWalkSpeed(float Value);
	UFUNCTION(BlueprintCallable)
		void IncreaseSpeed(float MultRatio);
	UFUNCTION(BlueprintCallable)
		void RestoreDefaultSpeed();
	
	float GetPhysicalDamageReduction(float DamagePoints);

	template<typename T>
	T* GetStatsOwner() const { return Cast<T>(GetOwner()); };
	template<typename T>
	T* GetStatsObject() const { return Cast<T>(CharacterStats); }

	virtual void UpdateMainStatValue(ECharStats Stat, float Value);		
};
