// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../BasePraiseCharacter.h"
#include "../BaseBotController.h"
#include "../../../Enums/AI/EMachineState.h"
#include "../../../Enums/AI/EBrainType.h"
#include "../../../Enums/AI/EBotClass.h"
#include "../../../Enums/CombatSystem/ETargetType.h"
#include "../../../Structs/Characters/FPlayerStats.h"
#include "Perception/PawnSensingComponent.h"
#include "../../../Interfaces/CombatSystem/Targeteable.h"
#include "BaseBotCharacter.generated.h"

UCLASS()
class PRAISE_API ABaseBotCharacter : public ABasePraiseCharacter
{
	GENERATED_BODY()
public:
	ABaseBotCharacter();
	FORCEINLINE EBotClass GetBotClass() const { return BotClass; }
	FORCEINLINE EBrainType GetBrainType() const { return BotBrainType; }
	FORCEINLINE class UBehaviorTree* GetBotDefaultBT() const { return BotDefaultBT; }
	// -- IDamageable
	virtual void GetDamage(float Damage, AActor* Damager, EAttackType AttackType = EAttackType::PRIMARY_ATTACK) override;
	virtual void GetWeaponDamage(float Damage, EAttackType AttackType, EWeaponSlot AttackerHand, ABasePraiseCharacter* Damager, AWeapon* DamagerWeapon) override;
	virtual bool HasBeenRecentlyDamaged() override;
	//--------------
	void OverrideBrainComp(EBrainType NewBrainType);
	virtual void OverrideBrainComp();
	bool DidBrainInit();
	FORCEINLINE FString GetStatsConfig() const { return StatsConfig; }
	FORCEINLINE bool CanAttackFactionChars() const { return bCanAttackFactionChars; }
	FORCEINLINE bool ShouldMove() const { return bShouldMove; }
	FORCEINLINE bool ShouldRun() const { return bShouldRun; }
	FORCEINLINE bool CanEverTravel() const { return bCanEverTravel; }
	FORCEINLINE bool CanEverPatrol() const { return bCanEverPatrol; }
	FORCEINLINE bool IgnoresCreatures() const { return bIgnoreCreatures; }
	FORCEINLINE bool OnlySubZonePatrols() const { return bOnlySubZonePatrols; }
	FORCEINLINE float GetMaxTimeInIdle() const { return MaxTimeInIdle; }
	FORCEINLINE float GetMaxTimeWandering() const { return MaxTimeWandering; }
	FORCEINLINE float GetMaxTimeChasing() const { return MaxTimeChasing; }
	FORCEINLINE float GetMaxTimeRunningAway() const { return MaxTimeRunningAway; }
	FORCEINLINE int GetNeutralHitsTolerance() const { return CharSpawnZone ? CharSpawnZone->GetNeutralHitsTolerance() : 1; }
	FORCEINLINE float GetPathFindingDistance() const { return PathFindingDistance; }
	FORCEINLINE void SetShouldMove(bool bValue) { bShouldMove = bValue; }
	FORCEINLINE void SetIgnoreCreatures(bool bValue) { bIgnoreCreatures = bValue; }
	


	void SetOnlySubZonePatrols(bool bValue);

	template<class T>
	T* GetBotController() const;
	template<class T>
	T* GetBotBrain() const;

	FORCEINLINE EMachineState GetCurrentAIState() const { return CurrentState; }

	UFUNCTION()
	virtual void OnAIStateUpdate(EMachineState NewState);
	virtual void BeginPlay() override;
	virtual bool TryInitBrain();
	virtual void HandleMessage(FTelegram& Msg) override;
	class UBotBrainComponent* GetBrain();
	virtual void TryLockTarget() override;
	virtual void StartPrimaryAttack() override;
	virtual void TriggerEvade() override;
	UFUNCTION()
		void TriggerEquipWeapons();

	virtual void TriggerWeaponSlot(EWeaponSlot Slot) override;
	virtual void HandleCharDeadNotify(AActor* Victim, AActor* Killer) override;
	bool CanReachLocation(FVector Loc);

protected:
	virtual void OnAnimStateUpdate(ECharAnimState State, bool bIsEnabled) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OverrideCharStatsComp() override;
	virtual void HandleSpeedChange(float DeltaTime) override;
	virtual void TrySetHandColliders() override;
	virtual void AddCharStatsBar() override;
	virtual void SetCharacterDead(AActor* Killer) override;
	virtual void OnInteractionBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	virtual void AddDefaultUnarmedWeapon();
	void HandleDamageReceived(float Damage, AActor* Damager);
	void ResetEquipWeapons();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		FString StatsConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI)
		bool bCanAttackFactionChars;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI)
		bool bShouldMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI)
		bool bShouldRun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bCanEverTravel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bCanEverPatrol;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float MaxTimeRunningAway;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float MaxTimeChasing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float MaxTimeWandering;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float MaxTimeInIdle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bIgnoreCreatures;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bIgnoreNeutrals;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bOnlySubZonePatrols;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float PathFindingDistance;
	UPROPERTY(EditAnywhere)
		TMap<int32, float> TotalReceivedDamageMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USphereComponent* RushCollider;
	UPROPERTY(EditAnywhere)
		TMap<int32, float> RecentlyReceivedDamageMap;

	UFUNCTION()
		void OnPawnSeen(class APawn* SeenPawn);
	UFUNCTION()
		void OnPawnHeard(class APawn* SeenPawn, const FVector& Location, float Volume);

	UFUNCTION()
		void OnKnownEnemyDead(AActor* Dead, AActor* Killer);

	class APraiseGameState* GameState;
	float DamageReceivedTimeStamp;
	float LastDamageReceivedTimeStamp;
	
	FPlayerStats GetDefaultBotStats();
	template<class T>
	void SetNewBrain();
	virtual bool SetSensor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		UPawnSensingComponent* BotSensor;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI)
		EMachineState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		EBotClass BotClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		EBrainType BotBrainType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		class UBehaviorTree* BotDefaultBT;
	virtual void HandleShouldRun();
	virtual void SetupCharWeaponSlots() override;
};

template<class T>
T* ABaseBotCharacter::GetBotController() const
{
	return GetController() ? Cast<T>(GetController()) : nullptr;
}

template<class T>
T* ABaseBotCharacter::GetBotBrain() const
{
	return GetBotController<ABaseBotController>() && GetBotController<ABaseBotController>()->GetBotBrain() ? Cast<T>(GetBotController<ABaseBotController>()->GetBotBrain()) : nullptr;
}

template<class T>
void ABaseBotCharacter::SetNewBrain()
{
	if (!GetBotController<ABaseBotController>()) return;

	GetBotController<ABaseBotController>()->OverrideBrainComponent<T>();
}


