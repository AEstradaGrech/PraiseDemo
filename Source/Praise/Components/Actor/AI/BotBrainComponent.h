// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../../Characters/AI/BaseBotController.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../../AI/CommonUtility/Navigation/WaypointHandler.h"
#include "../../../AI/CommonUtility/Navigation/AIWaypoint.h"
#include "../../../Interfaces/CombatSystem/Targeteable.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../../Praise.h"
#include "../../../Enums/CommonUtility/ECharVector.h"
#include "../../../AI/CommonUtility/BotEnvQuery/BotEnvQuerier.h"
#include "../../../Enums/AI/EChaseMode.h"
#include "../../../Enums/AI/EWanderMode.h"
#include "BotBrainComponent.generated.h"


DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnTargetUpdateSignature, AActor*, NewTarget, bool, bTrackDistance);
UCLASS()
class PRAISE_API UBotBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBotBrainComponent();
	
public:	
	
	virtual void ResetLoop();
	void PauseBrain(bool bPause, bool bResetLoop = false);
	AActor* GetPriorityTarget();
	void SetPriorityTarget(AActor* NewTarget);
	float LastAttackTimestamp;
	float LastEvadeTimestamp;
	float LastTargetUpdateTimestamp;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool InitBrain(ABaseBotController* OwnerController, ABaseBotCharacter* BrainOwner);

	FORCEINLINE bool IsQueryingEnv() const { return bIsQueryingEnv; }
	FORCEINLINE bool DidInit() const { return bDidInit; }
	FORCEINLINE bool IsBrainEnabled() const { return bIsBrainEnabled; }
	FORCEINLINE bool IsSprintStamEnabled() const { return bSprintStamEnabled; }
	virtual void SetBrainEnabled(bool bIsEnabled);
	FORCEINLINE void SetBestEvadeDirection(ECharVector Direction) { BestEvadeDirection = Direction; }
	FORCEINLINE void SetSprintStamEnabled(bool bValue) { bSprintStamEnabled = bValue; }
	
	FORCEINLINE class UBlackboardComponent* GetBotBB() const { return BotBB; }
	FORCEINLINE UBotEnvQuerier* GetEnvQuerier() const { return EnvQuerier; }

	FORCEINLINE bool HasBeenRecentlyDamaged() const { return BotBB ? BotBB->GetValueAsBool(BB_HAS_BEEN_DAMAGED) : false; }
	FORCEINLINE bool HasLimitedWanderingTime() const { return bHasLimitedWanderingTime; }
	FORCEINLINE float GetMaxWanderingTime() const { return MaxWanderingTime; }
	FORCEINLINE float GetBotFOV() const { return FieldOfView; }
	FORCEINLINE float GetPerceptionRadius() const { return DefaultPerceptionRadius; }
	FORCEINLINE float GetAttackAwaitSecs() const { return AttackAwaitSecs; }
	FORCEINLINE float GetEvadeAwaitSecs() const { return EvadeAwaitSecs; }
	FORCEINLINE float GetTargetUpdateSecs() const { return TargetUpdateSecs; }
	FORCEINLINE ECharVector GetBestEvadeDirection() const { return BestEvadeDirection; }
	FORCEINLINE float GetBotAllyFindingRange() const { return AllyFindingRange; }
	FORCEINLINE int GetMaxKnownWaypoints() const { return MaxKnownWaypoints; }
	FORCEINLINE TArray<AAIWaypoint*> GetKnownWaypoints() const { return KnownWaypoints; }
	FORCEINLINE UWaypointHandler* GetWaypointHandler() const { return BotWaypointHandler; }
	// FoEs
	FORCEINLINE TMap<AActor*, float> GetKnownItemsMap() const { return KnownItems; } 
	FORCEINLINE TArray<AActor*> GetKnownItems() const { TArray<AActor*> Keys; KnownItems.GetKeys(Keys); return Keys; };
	FORCEINLINE TArray<AActor*> GetKnownEnemies() const { TArray<AActor*> Keys; KnownEnemies.GetKeys(Keys); return Keys; }; 
	FORCEINLINE TArray<AActor*> GetKnownAllies() const { TArray<AActor*> Keys; KnownAllies.GetKeys(Keys); return Keys; };
	FORCEINLINE TArray<AActor*> GetNeutralDamagers() const { TArray<AActor*> Keys; NeutralDamagers.GetKeys(Keys); return Keys; }
	FORCEINLINE float GetMaxTimeBlocked() const { return MaxTimeBlocked; }
	FORCEINLINE float GetTimeBlocked() { return TimeBlocked; }
	FORCEINLINE void IncreaseTimeBlocked(float Amount) { TimeBlocked += Amount; }
	FORCEINLINE void SetTimeBlocked(float Value) { TimeBlocked = Value; }
	
	bool HasRoute(FString KeyName = WP_CURRENT_ROUTE);
	class ANodeWaypoint* GetCurrentDestination() const;
	class UAIRoute* GetRoute(FString KeyName = WP_CURRENT_ROUTE) const;
	void ClearRoute(FString KeyName = WP_CURRENT_ROUTE);
	TArray<class ANodeWaypoint*> GetRouteNodes(class ANodeWaypoint* InitialNode, class ANodeWaypoint* DestinationNode, TArray<class ANodeWaypoint*> AvailableNodes, TArray<ANodeWaypoint*>& OutSearchGraph, bool bIsDFS, bool bIsDescendingSearch = false);
	TArray<class ANodeWaypoint*> GetShortestRouteNodes(class ANodeWaypoint* InitialNode, class ANodeWaypoint* DestinationNode, TArray<class ANodeWaypoint*> AvailableNodes);
	TArray<class ANodeWaypoint*> GetKnownLocations() const;
	class ANodeWaypoint* GetRandomKnownLocation(TArray<class ANodeWaypoint*> Excluded = TArray<class ANodeWaypoint*>(), TArray<class ANodeWaypoint*> Locations = TArray<class ANodeWaypoint*>());
	void AddKnownLocation(class ANodeWaypoint* VipNode, bool bClearExistent = false);
	void AddKnownLocations(TArray<class ANodeWaypoint*> VipNodes, bool bClearExisstent = false);
	void AddWaypoints(FString CollectionLabel, TArray<AAIWaypoint*> WPs, bool bClearExistent = true);
	void AddKnowPathNodes(TArray<class ANodeWaypoint*> CurrentPathNodes, bool bClearExistent = true);
	void ClearKnownWaypoints() { KnownWaypoints.Empty(); }
	TArray<ITargeteable*> GetKnownCharTargets(bool bIncludeDeadTargets, bool bCheckOnlyEnemies = false, bool bCheckOnlyAllies = false);
	TArray<ITargeteable*> GetKnownEnemyTargets(bool bIncludeDeadTargets = false);
	TArray<ITargeteable*> GetKnownFriendlyTargets(bool bIncludeDeadTargets = false);
	ITargeteable* GetClosestTarget(TArray<ITargeteable*> Targets);


	FORCEINLINE bool HasKnownEnemies() const { return KnownEnemies.Num() > 0; }
	FORCEINLINE bool HasKnownAllies() const { return KnownAllies.Num() > 0; }

	bool TrySetRoute(class UAIRoute* Route, FString KeyName = FString(""));
	void AddKnownWaypoints(TArray<AAIWaypoint*> WPs, bool bOverride = false);

	FORCEINLINE void SetIsQueryingEnv(bool bValue) { bIsQueryingEnv = bValue; }
	FORCEINLINE void SetBotController(ABaseBotController* Controller) { BotController = Controller; }
	FORCEINLINE ABaseBotController* GetBotController() const { return BotController; }
	// ------------- BOT LOCOMOTION FLAGS AND SETTERS ----------------
	FORCEINLINE bool ShouldStayInIdle() const { return bShouldStayInIdle; }
	FORCEINLINE bool ShouldRun() const { return bShouldRun; }
	FORCEINLINE bool ShouldEquip() const { return bShouldEquip; }
	FORCEINLINE bool ShouldAttack() const { return bShouldAttack; }
	FORCEINLINE bool ShouldBlock() const { return bShouldBlock; }
	FORCEINLINE bool ShouldTarget() const { return bShouldTarget; }
	FORCEINLINE bool ShouldEvade() const { return bShouldEvade; }
	FORCEINLINE bool IsOnlyEnemyTargets() const { return BotBB ? BotBB->GetValueAsBool(BB_ONLY_ENEMY_TARGETS_ALLOWED) : false; }
	FORCEINLINE bool ShouldUnblockPath() const { return bShouldUnblockPath; }

	FORCEINLINE virtual void SetStayInIdle(bool Value) { bShouldStayInIdle = Value; };
	FORCEINLINE void SetShouldRun(bool Value) { bShouldRun = Value; }
	FORCEINLINE void SetShouldEquip(bool bValue) { bShouldEquip = bValue; }
	FORCEINLINE void SetShouldAttack(bool bValue) { bShouldAttack = bValue; }
	FORCEINLINE void SetShouldBlock(bool bValue) { bShouldBlock = bValue; }
	FORCEINLINE void SetShouldEvade(bool bValue) { bShouldEvade = bValue; }
	void SetShouldTarget(bool bValue);
	void SetShouldUnblockPath(bool bValue);
	void HandleBlockedPath(float DeltaTime);

	bool HasValidBBTarget(FName BBKey);
	void SetChaseMode(EChaseMode ChaseMode, float MaxChaseStateTime, bool OnChaseExitClearTarget = false);
	void SetWanderMode(EWanderMode WanderMode, float MaxWanderStateTime, float WanderingRadius = 1000.f);
	// --------------------------------------------------
		
	template<class T> T* GetBot() { return Cast<T>(Bot); }

	bool TryAddKnownItem(ITargeteable* NewItem);
	bool TryAddNewTarget(ITargeteable* Target); 
	bool TryRemoveKnownTarget(ITargeteable* Target);
	bool TryRemoveTarget(ITargeteable* Target, TMap<AActor*, float>& Map);
	bool TryAddTarget(ITargeteable* Target, TMap<AActor*, float>& Map);
	bool TryAddCharTarget(ABasePraiseCharacter* Character, bool bIsEnemy);
	bool IsValidTarget(APawn* Target) const;

	ITargeteable* GetClosestFoE(bool bEnemy = true, bool bAllowDeadTargets = false);
	ITargeteable* GetClosestItem();

	void RegisterReceivedDamage(float Damage, int32 DamagerID);
	void ProcessRecentDamage();
	void ProcessTotalDamage();

	UPROPERTY(VisibleAnywhere, Category = AI)
		TArray<FVector> NearbyAllyLocations;

	UPROPERTY(VisibleAnywhere, Category = AI)
		TArray<AActor*> NearbyTargets;

	UFUNCTION()
		void ForceStopBrain();

	float GetTimeSinceGameStart() const;
	AActor* GetClosestTarget(TArray<AActor*> Targets, ECharVector LocationCondition = ECharVector::NONE, bool bAllowDeadTargets = false);

	FOnTargetUpdateSignature OnTargetUpdate;
	void ClearBBTarget(AActor* Target);
	void ClearBBTarget();
	virtual void SetupBB(); 
	bool BeginEnvQuery(FEnvQueryConfig& QueryConfig, TArray<FVector>& QueryResults, int MaxResults = 1);
protected:
	// Called when the game starts
	virtual void SetDefaults();
	virtual void BeginPlay() override;
	virtual void ClearBB();
	bool TryInitCommonBB();
	void TryClearBBTarget(FName BBKey, AActor* Target);

	UFUNCTION()
		void OnPerceptionCheck();
	UFUNCTION()
		void OnTargetRecallCheck();

	void ClearTargetsMap(TMap<AActor*, float>& Map, float RecallSeconds);
	
	bool GetRouteNodes_DSF(class ANodeWaypoint* Vertex, class ANodeWaypoint* TargetNode, TArray<class ANodeWaypoint*>& PathNodes, TArray<class ANodeWaypoint*>& CheckedNodes, TArray<class ANodeWaypoint*>& Nodes, bool bIsDescendingSearch);
	bool GetRouteNodes_BSF(TArray<ANodeWaypoint*>& LevelNodes, class ANodeWaypoint* TargetNode, TArray<class ANodeWaypoint*>& PathNodes, TArray<class ANodeWaypoint*>& CheckedNodes, TArray<class ANodeWaypoint*>& Nodes, bool bIsDescendingSearch);

	UPROPERTY(VisibleAnywhere)
		ABaseBotCharacter* Bot;
	UPROPERTY(VisibleAnywhere)
		ABaseBotController* BotController;
	UPROPERTY(VisibleAnywhere, Category = AI)
		UBlackboardComponent* BotBB;
	UPROPERTY(VisibleAnywhere, Category = AI)
		class UBlackboardData* BBData;
	UPROPERTY(VisibleAnywhere)
		UBotEnvQuerier* EnvQuerier;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI, meta = (AllowPrivateAccess = "true"))
		bool bIsQueryingEnv = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI, meta = (AllowPrivateAccess = "true"))
		bool bDidInit = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI, meta = (AllowPrivateAccess = "true"))
		bool bShouldStayInIdle = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI, meta = (AllowPrivateAccess = "true"))
		bool bSprintStamEnabled = false;
	UPROPERTY(EditAnywhere, Category = AI)
		float FieldOfView = 80.f;
	UPROPERTY(EditAnywhere, Category = AI)
		float SightRadius = 4000.f;
	UPROPERTY(EditAnywhere, Category = AI)
		float DefaultPerceptionRadius = 500.f;
	UPROPERTY(EditAnywhere, Category = AI)
		float PerceptionCheckSeconds = 5.f;
	UPROPERTY(EditAnywhere, Category = AI)
		float AttackAwaitSecs = 1.f;
	UPROPERTY(EditAnywhere, Category = AI)
		float EvadeAwaitSecs = 1.f;
	UPROPERTY(EditAnywhere, Category = AI)
		float TargetUpdateSecs = 1.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI, meta = (AllowPrivateAccess = "true"))
		float MaxTimeBlocked;

	UPROPERTY(EditAnywhere, Category = AI)
		ECharVector BestEvadeDirection;
	// for AI_Msg_Commands
	UPROPERTY(EditAnywhere, Category = AI)
		float AllyFindingRange = 3000.f;
	UPROPERTY(VisibleAnywhere, Category = AI, meta = (ClampMin = "1", ClampMax="20"))
		int MaxKnownWaypoints = 10;
	UPROPERTY(VisibleAnywhere, Category = AI)
		TArray<AAIWaypoint*> KnownWaypoints;
	UPROPERTY()
		UWaypointHandler* BotWaypointHandler;

	TMap<FVector, float> UnblockPathLocs;
	int UnblockPathRetries;
	int MaxUnblockPathRetries;
	float UnblockPathLocRecallSecs;
	float UnblockPathRecallTimerSecs;
	
	bool bIsBrainEnabled;
	float TimeBlocked;
	FTimerHandle UnblockPathLocsCheckTimer;
	UFUNCTION()
		void OnUnblockPathLocsRecall();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
		TMap<AActor*, float> KnownItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
		TMap<AActor*, float> KnownEnemies;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
		TMap<AActor*, float> KnownAllies;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
		TMap<AActor*, int> NeutralDamagers;

	//TODO KnownVeryImportanPlaces
	FTimerHandle LocationsRecallCheckTimer;
	FTimerHandle TargetRecallCheckTimer;
	FTimerHandle NeutralDamagersRecallCheckTimer;
	FTimerHandle ResetDamageTimeHandle;
	FTimerHandle PerceptionCheckTimerHandle;
	FTimerHandle LastKnownEnemyCheckTimer;

	void AddNeutralDamager(AActor* Damager, int ExtraHits = 0);

	UFUNCTION()
		void ClearLastKnownEnemyLoc();
	UFUNCTION()
		void TryResetRecentDamageMap();
	UFUNCTION()
		void ClearNeutralDamgers();

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		float NeutralDamagersRecallCheckTimers;
	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		float DamageAlertResetTime;
	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		float RecentDamageResetTime;
	UPROPERTY(EditAnywhere)
		TMap<int32, float> TotalReceivedDamageMap;
	UPROPERTY(EditAnywhere)
		TMap<int32, float> RecentlyReceivedDamageMap;

	float DamageReceivedTimeStamp;
	float LastDamageReceivedTimeStamp;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true", ClampMin = "10"))
		float MaxWanderingTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bHasLimitedWanderingTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bShouldRun;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bShouldEquip;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bShouldAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bShouldBlock;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bShouldTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bShouldEvade;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bShouldUnblockPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bOnlyEnemyTargets;
	
private:
	
	
	void AddRecallCheckTimer();
	void AddLastKnownEnemyLocCheckTimer();

	void ProcessItem(class ITargeteable* Item);
	void ProcessCharacter(class ABasePraiseCharacter* Character);
};
