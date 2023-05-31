// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/SkeletalMesh.h"
#include "../Interfaces/CombatSystem/Targeteable.h"
#include "../Enums/CombatSystem/ETargetType.h"
#include "../Enums/CombatSystem/EArmorType.h"
#include "../Enums/Characters/ECharStatus.h"
#include "../Structs/CombatSystem/Armors/FArmorsDTStruct.h"
#include "../Structs/CombatSystem/Armors/FArmorArtDTStruct.h"
#include "Configs/ArmorConfig.h"
#include "Armor.generated.h"

UCLASS()
class PRAISE_API AArmor : public AActor, public ITargeteable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArmor();

	FORCEINLINE EArmorType GetArmorType() const { return ArmorType; }
	FORCEINLINE int32 GetArmorLvl() const { return Lvl; }
	FORCEINLINE float GetArmorAbsorption() const { return Absorption; }
	FORCEINLINE float GetWeight() const { return Weight; }
	FORCEINLINE bool DidInit() const { return bDidInit; }
	FORCEINLINE bool IsFullSet() const { return bIsFullSet; }
	FORCEINLINE bool IsBroken() const { return bIsBroken; }
	FORCEINLINE bool IsEquiped() const { return bIsEquiped; }

	FORCEINLINE void SetStatsConfigID(int32 Value) { StatsConfigID = Value; }

	float GetCurrentAbsorption();
	float GetArmorState() const;
	float GetArmorDurability() const;
	float GetArmorStateAsRatio() const;
	float GetDurabilityAsRatio() const;
	bool HasOwner() const { return GetOwner() != nullptr; }
	bool TryInitStats();
	bool TrySetupArmorArt();
	bool InitArmor(EArmorType Type, FString Model);
	// ITargeteable
	virtual int32 GetTargetID() const override;
	virtual ETargetType IsTargetType() const override;
	virtual ECharStatus GetCharStatus() const override;
	
	virtual bool IsDead() const override;
	//
	void ApplyArmorStateReduction(float HandledDamage);
	void Equip(bool bEquip, FName Socket, bool bPlaySFX = true);
	void Equip(class ABasePraiseCharacter* Char);
	void OnArmorDropped();
	void OnArmorPickup(class ABasePraiseCharacter* NewOwner);
	float HandleDamage(float DamagePoints);
	UArmorConfig* GetCurrentStatsAsConfig();
	template<typename T>
	T* GetStatsConfigAs();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void ResetToCurrentConfig();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Armor)
		class UArmorsDBComponent* ArmorsDB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool bDidInit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		int32 StatsConfigID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool bIsEquiped;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		ETargetType TargetType;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		ECharStatus ItemStatus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		int32 ModifierType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		EArmorType ArmorType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FString ArmorModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FString PathToMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FString PathToDroppedArmorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FVector DroppedMeshScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool bIsFullSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool bIsBroken;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		int32 Lvl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float BaseXP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float Absorption;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float ArmorState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float StateReductionConst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float RemainingDmgReductionMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float Durability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float ChanceToBreak;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool IsConfigBased;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool AppliesOnConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		TArray<int32> ArmorModifiers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		TArray<int32> ArmorAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		int32 ArmorSkinsID;

	UArmorConfig* StatsConfig;
	FArmorArtDTStruct* CurrentSkin;

	UFUNCTION()
		virtual void OnInteractionColBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnInteractionColEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = ArmorColliders, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* DroppedArmorCollider;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = ArmorColliders, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* InteractionCollider;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = ArmorColliders, meta = (AllowPrivateAccess = "true"))
		float InteractionColRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		FVector DroppedArmorBoxExtents;

	void SetArmorState(float Value);

	float GetArmorStateFactor() const;
	float GetArmorStateReduction() const;

	bool TryAddBaseStats();
	bool TryAddStatsConfig();
	bool TryAddStatModifiers();

	void FullOverwriteStats(FArmorsDTStruct* NewStats);
	void OverwriteStats(FArmorsDTStruct* NewStats, bool bAppliesOnConfig = true);
	void PlusStats(FArmorsDTStruct* Addition, bool bAppliesOnConfig = false);

	void ApplyModifier(FArmorsDTStruct* Modifier, bool bAppliesOnConfig = true);
	void ApplyMultiplier(FArmorsDTStruct* Multiplier, bool bAppliesOnConfig = true);
	void ApplyPercentage(FArmorsDTStruct* Modifier, bool bAppliesOnConfig = true, bool bIsBasedOnCurrentStats = false);
	
	bool TrySetupArmorColliders(FArmorArtDTStruct* ArtData = nullptr);
	void UpdateStatsFromConfig(FArmorsDTStruct* Config = nullptr, bool bIsAdditive = false, bool bAppliesOnConfig = true);

	void SetupCurrentStatsArrays(FArmorsDTStruct* Config, bool bIsAdditive = false);

	void HandleEnableCollider(UBoxComponent* Collider, bool bEnable);

	private:
		USkeletalMesh* GetSkinMesh(FString PathToMesh);
};

template<typename T>
inline T* AArmor::GetStatsConfigAs()
{
	if (!StatsConfig) return nullptr;

	return Cast<T>(StatsConfig);
}
