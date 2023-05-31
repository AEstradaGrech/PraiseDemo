// Fill out your copyright notice in the Description page of Project Settings.


#include "BotSpawnerComponent.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../Characters/AI/BotCharacters/CitizenBotCharacter.h"
#include "../../Characters/AI/BotCharacters/BasicZombieCharacter.h"
#include "../../AI/CommonUtility/Factions/GameFaction.h"
#include "Engine/DataTable.h"
#include "../../Characters/AI/BotCharacters/FightingBotCharacter.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"

// Sets default values for this component's properties
UBotSpawnerComponent::UBotSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MachineStatesFactory = CreateDefaultSubobject<UMachineStatesFactoryComponent>(TEXT("Machine States Factory"));

	static const ConstructorHelpers::FObjectFinder<UDataTable> StatConfigsDT(TEXT("DataTable'/Game/Core/DataTables/Characters/DT_BotStatsConfig.DT_BotStatsConfig'"));

	if (StatConfigsDT.Succeeded())
		BotStatsConfigDT = StatConfigsDT.Object;

	ConstructorHelpers::FClassFinder<ABaseBotCharacter> CitizenCharClass (TEXT("/Game/Core/Blueprints/Characters/Bots/BP_CitizenBotCharacter"));

	if (CitizenCharClass.Succeeded())
		BotBPs.Add(BP_CITIZEN_BOT, CitizenCharClass.Class);

	ConstructorHelpers::FClassFinder<ABaseBotCharacter> GuardCharClass(TEXT("/Game/Core/Blueprints/Characters/Bots/BP_GuardBotCharacter"));

	if (GuardCharClass.Succeeded())
		BotBPs.Add(BP_GUARD_BOT, GuardCharClass.Class);

	ConstructorHelpers::FClassFinder<ABaseBotCharacter> SeekerCharClass(TEXT("/Game/Core/Blueprints/Characters/Bots/BP_SeekerDemonBotCharacter"));

	if (SeekerCharClass.Succeeded())
		BotBPs.Add(BP_SEEKER_DEMON_BOT, SeekerCharClass.Class);

	ConstructorHelpers::FClassFinder<ABaseBotCharacter> PlayerSeekerCharClass(TEXT("/Game/Core/Blueprints/Characters/Bots/BP_PlayerSeekerBotCharacter"));

	if (PlayerSeekerCharClass.Succeeded())
		BotBPs.Add(BP_SEEKER_BOT, PlayerSeekerCharClass.Class);

	ConstructorHelpers::FClassFinder<ABaseBotCharacter> BasicZombieCharClass(TEXT("/Game/Core/Blueprints/Characters/Bots/BP_BasicZombieBotCharacter"));

	if (BasicZombieCharClass.Succeeded())
		BotBPs.Add(BP_BASIC_ZOMBIE_BOT, BasicZombieCharClass.Class);
	// ...
}

void UBotSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();


	// ...

}

ABaseBotCharacter* UBotSpawnerComponent::GetDummyBot()
{
	if (!ensure(GetWorld() != nullptr)) return nullptr;
	
	if (!BotBPs.Contains(BP_CITIZEN_BOT)) return nullptr;

	FActorSpawnParameters SpawnParams;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector SpawnLocation = GetDefaultSpawnLocation();
	
	ABaseBotCharacter* Bot = GetWorld()->SpawnActor<ABaseBotCharacter>(BotBPs[BP_CITIZEN_BOT], SpawnLocation == FVector::ZeroVector ? DefaultMapSpawnPostion : SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (!Bot) return nullptr;

	return Bot;
}

bool UBotSpawnerComponent::EnsureSetup()
{
	if(CreateBotFunctionsMap.Num() <= 0)
		RegisterBots();

	if (!MachineStatesFactory)
	{
		MachineStatesFactory = NewObject<UMachineStatesFactoryComponent>(this, FName("Machine States Factory"));
		if (!MachineStatesFactory) return false;
		
		MachineStatesFactory->RegisterComponent();
		MachineStatesFactory->InitializeComponent();
		MachineStatesFactory->EnsureStatesRegistration();
		
		if (!MachineStatesFactory->HasRegisteredStates()) return false;
	}

	return CommonWaypoints.Num() > 0;
}


TSubclassOf<ABaseBotCharacter> UBotSpawnerComponent::GetBotBP(FString BPName)
{
	TArray<FString> Keys;
	BotBPs.GetKeys(Keys);

	for (FString Key : Keys)
		if (Key.Contains(BPName))
			return BotBPs[Key];

	return nullptr;
}

ABaseBotCharacter* UBotSpawnerComponent::GetBot(EBotClass BotClass)
{
	return CreateBotFunctionsMap.Contains(BotClass) ? (this->*CreateBotFunctionsMap[BotClass])() : nullptr;
}

ABaseBotCharacter* UBotSpawnerComponent::GetBot(EBotClass BotClass, EBrainType BrainType)
{
	ABaseBotCharacter* Bot = GetBot(BotClass);

	if (!Bot) return nullptr;

	Bot->OverrideBrainComp(BrainType);

	return Bot;
}

ABaseBotCharacter* UBotSpawnerComponent::GetBot(FString BotBP, FVector SpawnPosition)
{
	if (!BotBPs.Contains(BotBP)) return nullptr;

	if (!ensure(GetWorld() != nullptr)) return nullptr;

	return BotBPs.Contains(BotBP) ? GetBot(BotBPs[BotBP], SpawnPosition) : nullptr;
}

ABaseBotCharacter* UBotSpawnerComponent::GetBot(TSubclassOf<ABasePraiseCharacter> BotBP, FVector SpawnPosition)
{
	if (!ensure(GetWorld() != nullptr)) return nullptr;

	FActorSpawnParameters SpawnParams;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = SpawnPosition == FVector::ZeroVector ? GetDefaultSpawnLocation() : SpawnPosition;

	ABaseBotCharacter* Bot = GetWorld()->SpawnActor<ABaseBotCharacter>(BotBP, SpawnLocation == FVector::ZeroVector ? DefaultMapSpawnPostion : SpawnLocation, FRotator::ZeroRotator, SpawnParams); // esto no tiene Mesh. Cachear BP

	if (!Bot) return nullptr;
	
	return Bot;
}
ABaseBotCharacter* UBotSpawnerComponent::GetBot(FString BotBPName, AGameFaction* BotFaction, AFactionZone* SpawnZone, FVector SpawnPosition)
{
	if (BotBPs.Num() <= 0) return nullptr;

	if (!BotBPs.Contains(BotBPName)) return false;

	return GetBot(BotBPs[BotBPName], BotFaction, SpawnZone, SpawnPosition);
}
ABaseBotCharacter* UBotSpawnerComponent::GetBot(TSubclassOf<ABasePraiseCharacter> BotBP, AGameFaction* BotFaction, AFactionZone* SpawnZone, FVector SpawnPosition)
{
	if (!ensure(GetWorld() != nullptr)) return nullptr;

	FActorSpawnParameters SpawnParams;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = SpawnPosition == FVector::ZeroVector ? GetDefaultSpawnLocation() : SpawnPosition;

	ABaseBotCharacter* Bot = GetWorld()->SpawnActor<ABaseBotCharacter>(BotBP, SpawnLocation == FVector::ZeroVector ? DefaultMapSpawnPostion : SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (!Bot) return nullptr;

	AddBotConfig(Bot);

	if (Bot->GetBotBrain<UStateMachineBrainComponent>())
		Bot->GetBotBrain<UStateMachineBrainComponent>()->SetStatesFactory(MachineStatesFactory);
	
	Bot->SetFaction(BotFaction);
	Bot->SetCharSpawnZone(SpawnZone);

	BotFaction->GetMsgDispatcher()->OnMsgBroadcast.AddDynamic(Bot, &ABasePraiseCharacter::HandleMessage);
	SpawnZone->GetMsgDispatcher()->OnMsgBroadcast.AddDynamic(Bot, &ABasePraiseCharacter::HandleMessage);

	return Bot;
}
// Called when the game starts


FVector UBotSpawnerComponent::GetDefaultSpawnLocation(bool bRandomWaypoint)
{	
	if (CommonWaypoints.Num() <= 0)  
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIWaypoint::StaticClass(), OutActors);
	
		if (OutActors.Num() > 0)
			return OutActors[0]->GetActorLocation();
	}

	else return bRandomWaypoint && CommonWaypoints.Num() > 0 ? CommonWaypoints[FMath::RandRange(0, CommonWaypoints.Num() -1)]->GetActorLocation() : CommonWaypoints.Num() > 0 ? CommonWaypoints[0]->GetActorLocation() : FVector::ZeroVector;
	
	return FVector();
}

FBotStats* UBotSpawnerComponent::GetStatsConfig(FString ConfigName)
{
	static const FString ContextString = FString("Getting config --> " + ConfigName);

	try
	{
		TArray<FBotStats*> Rows;

		BotStatsConfigDT->GetAllRows(ContextString, Rows);

		for (FBotStats* Row : Rows)
			if (Row->CharName == ConfigName)
				return Row;

	}
	catch (std::exception e)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: Exception Message --> ") + e.what());
	}

	return nullptr;
}

void UBotSpawnerComponent::RegisterBots()
{
	RegisterBot<ACitizenBotCharacter>(EBotClass::CITIZEN);
	RegisterBot<AFightingBotCharacter>(EBotClass::FIGHTER);
	RegisterBot<ABasicZombieCharacter>(EBotClass::UNDEAD);
}

void UBotSpawnerComponent::AddBotConfig(ABaseBotCharacter* Bot)
{
	if (!Bot->GetCharStats()) return;

	FString ConfigName = Bot->GetStatsConfig().IsEmpty() ? FString("Default") : Bot->GetStatsConfig();

	FBotStats* Config = GetStatsConfig(ConfigName);

	if (!Config) return;

	Bot->GetCharStats()->InitializeStatsObject(Config);

	Bot->GetCharacterMovement()->MaxWalkSpeed = Bot->GetCharStats()->GetDefaultWalkSpeed();
}






