// Fill out your copyright notice in the Description page of Project Settings.


#include "../Player/PraisePlayerCharacter.h"
#include "../Player/PraisePlayerController.h"
#include "../../Components/Actor/AnimInstances/PraiseAnimInstance.h"
#include "../../Components/Actor/CharStats/PlayerStatsComponent.h"
#include "../../Weapons/Fist.h"
#include "../../Weapons/MeleeWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../Weapons/Firearm.h"
#include "Gameframework/GameStateBase.h"
#include "../AI/BotCharacters/BaseBotCharacter.h"
#include "../../PraiseGameInstance.h"
#include "Components/AudioComponent.h"
#include "../../Enums/CommonUtility/EGameMenu.h"
#include "Sound/SoundCue.h"
#include "../../Structs/CommonUtility/FOsc.h"

APraisePlayerCharacter::APraisePlayerCharacter() : Super()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Char Audio Component"));
	AudioComp->AttachTo(RootComponent);
	FootstepsAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Footsteps Audio Component"));
	FootstepsAudioComp->AttachTo(RootComponent);

	CharWeaponsFactory = CreateDefaultSubobject<UWeaponsFactoryComponent>(TEXT("Weapons Factory"));
	CharArmorsFactory = CreateDefaultSubobject<UArmorsFactoryComponent>(TEXT("Armors Factory"));

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DEFAULT_CAMERA_ZOOM;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCamera->bUsePawnControlRotation = false;

	TargetType = ETargetType::PLAYER;

	bIsPressingWeaponSlotKey = false;
	SecondsToDropWeapon = 2.f;
	SecondsPressingWeaponKey = 0;
	bCanChangeDefaultSkin = true;

	static ConstructorHelpers::FObjectFinder<USoundCue> CombatQ (TEXT("SoundCue'/Game/Core/Audio/Combat/CombatFX.CombatFX'"));

	if (CombatQ.Succeeded())
		CombatSoundCue = CombatQ.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> MiscQ(TEXT("SoundCue'/Game/Core/Audio/Misc/MiscSoundCue.MiscSoundCue'"));

	if (MiscQ.Succeeded())
		MiscSoundCue = MiscQ.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CharQ(TEXT("SoundCue'/Game/Core/Audio/Combat/CharFX.CharFX'"));

	if (CharQ.Succeeded())
		CharSoundCue = CharQ.Object;
}

// SETUPf
void APraisePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("InGameMenu", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::HandleInGameMenu);

	PlayerInputComponent->BindAxis("MoveForward", this, &APraisePlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APraisePlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MouseX", this, &APraisePlayerCharacter::AddMouseHorizontalInput);
	PlayerInputComponent->BindAxis("MouseY", this, &APraisePlayerCharacter::AddMouseVerticalInput);

	PlayerInputComponent->BindAction("Spacebar", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Spacebar", EInputEvent::IE_Released, this, &APraisePlayerCharacter::EndJump);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APraisePlayerCharacter::EndCrouch);

	PlayerInputComponent->BindAction("PrimaryAttack", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::StartPrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryAttack", EInputEvent::IE_Released, this, &APraisePlayerCharacter::EndPrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::StartSecondaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", EInputEvent::IE_Released, this, &APraisePlayerCharacter::EndSecondaryAttack);
	PlayerInputComponent->BindAction("Kick", EInputEvent::IE_Pressed, this, &ABasePraiseCharacter::DropKick);

	PlayerInputComponent->BindAction("Parry", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::StartParry);
	PlayerInputComponent->BindAction("Parry", EInputEvent::IE_Released, this, &APraisePlayerCharacter::EndParry);
	PlayerInputComponent->BindAction("Block", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::StartBlocking);
	PlayerInputComponent->BindAction("Block", EInputEvent::IE_Released, this, &APraisePlayerCharacter::EndBlocking);
	PlayerInputComponent->BindAction("ReleaseBlock", EInputEvent::IE_Released, this, &APraisePlayerCharacter::EnsureReleaseBlock);
	PlayerInputComponent->BindAction("LockTarget", EInputEvent::IE_Released, this, &APraisePlayerCharacter::TryLockTarget);
	PlayerInputComponent->BindAction("SkipTarget_L", EInputEvent::IE_Released, this, &APraisePlayerCharacter::TrySkipTarget_L);
	PlayerInputComponent->BindAction("SkipTarget_R", EInputEvent::IE_Released, this, &APraisePlayerCharacter::TrySkipTarget_R);

	PlayerInputComponent->BindAction("Equip", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::TriggerEquipWeapons);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABasePraiseCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABasePraiseCharacter::EndSprint);

	PlayerInputComponent->BindAction<FOnTriggerSlotWeaponSignature>("WeaponSlot1", EInputEvent::IE_Released, this, &APraisePlayerCharacter::TriggerWeaponSlot, EWeaponSlot::PRIMARY_WEAPON);
	PlayerInputComponent->BindAction<FOnTriggerSlotWeaponSignature>("WeaponSlot2", EInputEvent::IE_Released, this, &APraisePlayerCharacter::TriggerWeaponSlot, EWeaponSlot::SECONDARY_WEAPON);
	PlayerInputComponent->BindAction<FOnTriggerSlotWeaponSignature>("WeaponSlot3", EInputEvent::IE_Released, this, &APraisePlayerCharacter::TriggerWeaponSlot, EWeaponSlot::EXTRA_SLOT_1);
	PlayerInputComponent->BindAction<FOnTriggerSlotWeaponSignature>("WeaponSlot4", EInputEvent::IE_Released, this, &APraisePlayerCharacter::TriggerWeaponSlot, EWeaponSlot::EXTRA_SLOT_2);
																															  
	PlayerInputComponent->BindAction<FOnDropSlotWeaponSignature>("DropWeaponSlot1", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::DropWeaponSlot, EWeaponSlot::PRIMARY_WEAPON);
	PlayerInputComponent->BindAction<FOnDropSlotWeaponSignature>("DropWeaponSlot2", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::DropWeaponSlot, EWeaponSlot::SECONDARY_WEAPON);
	PlayerInputComponent->BindAction<FOnDropSlotWeaponSignature>("DropWeaponSlot3", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::DropWeaponSlot, EWeaponSlot::EXTRA_SLOT_1);
	PlayerInputComponent->BindAction<FOnDropSlotWeaponSignature>("DropWeaponSlot4", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::DropWeaponSlot, EWeaponSlot::EXTRA_SLOT_2);

	PlayerInputComponent->BindAction<FOnTriggerPickupItemSignature>("PickupItem", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::TriggerPickupItem, EWeaponSlot::INVALID);
	PlayerInputComponent->BindAction<FOnTriggerPickupItemSignature>("PickupWeaponSlot1", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::TriggerPickupItem, EWeaponSlot::PRIMARY_WEAPON);
	PlayerInputComponent->BindAction<FOnTriggerPickupItemSignature>("PickupWeaponSlot2", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::TriggerPickupItem, EWeaponSlot::SECONDARY_WEAPON);
	PlayerInputComponent->BindAction<FOnTriggerPickupItemSignature>("PickupWeaponSlot3", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::TriggerPickupItem, EWeaponSlot::EXTRA_SLOT_1);
	PlayerInputComponent->BindAction<FOnTriggerPickupItemSignature>("PickupWeaponSlot4", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::TriggerPickupItem, EWeaponSlot::EXTRA_SLOT_2);

	//Dev Only
	PlayerInputComponent->BindAction("TestKey", EInputEvent::IE_Pressed, this, &APraisePlayerCharacter::TriggerTestKey);
}

void APraisePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APraisePlayerCharacter::OverrideCharStatsComp()
{
	UPlayerStatsComponent* PlayerStats = NewObject<UPlayerStatsComponent>(this);

	if (!PlayerStats) return;

	PlayerStats->InitializePlayerStatsObject(GetDefaultPlayerStats());

	CharStats = PlayerStats;

	CharStats->RegisterComponent();
	CharStats->InitializeComponent();

	GetCharacterMovement()->MaxWalkSpeed = CharStats->GetWalkSpeed();
}

ECharVector APraisePlayerCharacter::GetCharMovementVector()
{
	if (GetVelocity().Size() > 0)
	{
		FVector Velo = GetVelocity().GetSafeNormal();
		
		Velo.Z = 0;

		float Degs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), Velo)));

		FVector Cross = FVector::CrossProduct(GetActorForwardVector(), Velo);

		if (Degs <= 25)
			return ECharVector::FWD;
		
		if (Degs >= 160)
			return ECharVector::BWD;

		if (Degs >= 80 && Degs <= 100.f)
			return FMath::Sign(Cross.Z) > 0 ? ECharVector::RIGHT : ECharVector::LEFT;
		
		if (Degs < 80 && Degs > 25)
			return FMath::Sign(Cross.Z) > 0 ? ECharVector::FWD_R : ECharVector::FWD_L;

		if (Degs < 160 && Degs > 100)
			return FMath::Sign(Cross.Z) > 0 ? ECharVector::BWD_R : ECharVector::BWD_L;

		return ECharVector::NONE;
	}

	else return ECharVector::NONE;
}

void APraisePlayerCharacter::TrySetHandColliders()
{
	HandCollider_L = InitHandCollider(FString("Hand Collider L"), FName("Hand_L"));
	HandCollider_R = InitHandCollider(FString("Hand Collider R"), FName("Hand_R"));
	KickCollider_R = InitHandCollider(FString("Kick Collider R"), FName("Foot_R"));
	KickCollider_L = InitHandCollider(FString("Kick Collider L"), FName("Foot_L"));

	HandCollider_L->OnComponentBeginOverlap.AddDynamic(this, &APraisePlayerCharacter::OnFistBeginOverlap);
	HandCollider_R->OnComponentBeginOverlap.AddDynamic(this, &APraisePlayerCharacter::OnFistBeginOverlap);
	KickCollider_R->OnComponentBeginOverlap.AddDynamic(this, &APraisePlayerCharacter::OnKickBeginOverlap);
	KickCollider_L->OnComponentBeginOverlap.AddDynamic(this, &APraisePlayerCharacter::OnKickBeginOverlap);

}

void APraisePlayerCharacter::SetupCharWeaponSlots()
{
	if (!CharWeaponsFactory) return;

	for (int i = 0; i < 4; i++) 
	{
		UCharWeaponSlot* Slot = NewObject<UCharWeaponSlot>(this);
		Slot->SetupSlot(EWeaponSlot(i));
		
		if (i == (int)EWeaponSlot::PRIMARY_WEAPON) 
		{
			EHolsterPosition PrimaryWeaponHolster = EHolsterPosition::THIGH_R;
			AWeapon* DevWeapon_R = CharWeaponsFactory->SpawnWeapon<AMeleeWeapon>(PrimaryWeaponHolster, EWeaponType::BLUNT, DEFAULT_BLUNT_1H_MODEL);

			if (DevWeapon_R && DevWeapon_R->DidInit()) 
			{
				AttachWeapon(DevWeapon_R, PrimaryWeaponHolster);
				Slot->SetSlotWeapon(DevWeapon_R, PrimaryWeaponHolster);
			}
		}
		
		if (i == (int)EWeaponSlot::SECONDARY_WEAPON) 
		{
			EHolsterPosition SecondaryWeaponHolster = EHolsterPosition::SHOULDER_R;
			AWeapon* DevWeapon_L = CharWeaponsFactory->SpawnWeapon<AFirearm>(SecondaryWeaponHolster, EWeaponType::FIREARM, DEFAULT_BAYONET_RIFLE_MODEL, 2);
					
			if (DevWeapon_L && DevWeapon_L->DidInit()) 
			{
				AttachWeapon(DevWeapon_L, SecondaryWeaponHolster);
				Slot->SetSlotWeapon(DevWeapon_L, SecondaryWeaponHolster);
			}
		}

		if(!CharWeaponSlots.Contains((EWeaponSlot)i))
			CharWeaponSlots.Add((EWeaponSlot)i, Slot);
	}
}


//Devonly (until CharCreatorMenu & SaveGame implementation)
FPlayerStats APraisePlayerCharacter::GetDefaultPlayerStats()
{
	FPlayerStats Stats;
	Stats.Strenght = 5;
	Stats.Constitution = 5;
	Stats.Dextrity = 5;
	Stats.Intelligence = 5;
	Stats.Charisma = 5;
	Stats.Wisdom = 5;
	Stats.Luck = 5;
	Stats.Level = 1;
	Stats.Class = ECharClass::NONE;

	return Stats;
}
void APraisePlayerCharacter::TriggerTestKey()
{
	if (!CharWeaponSlots.Contains(EWeaponSlot::PRIMARY_WEAPON)) return;

	if (CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty()) return;

	GetWeaponDamage(-TestDamage, EAttackType::PRIMARY_ATTACK, EWeaponSlot::PRIMARY_WEAPON, this, CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon());
}
// ---- end dev only
void APraisePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetController() && GetController()->IsA<APlayerController>())
	{
		FInputModeGameOnly InputMode;

		Cast<APlayerController>(GetController())->SetInputMode(InputMode);

		Cast<APlayerController>(GetController())->bShowMouseCursor = false;
	}

	bInGameMenuEnabled = false;
	bIsHoldingPrimary = false;
	bIsHoldingSecondary = false;
	bIsZoomRequested = false;
	bCanAttack = true;
	CameraZoom = 150.f;
	CameraZoomSpeed = 3.f;
	
	if (CameraOffset == 0.f)
		CameraOffset = DEFAULT_CAMERA_OFFSET;

	PlayerCamera->SetWorldLocation(PlayerCamera->GetComponentLocation() + (PlayerCamera->GetRightVector() * GetClampedcameraOffset()));
	
	if (GetGameInstance() && GetGameInstance()->IsA<UPraiseGameInstance>())
	{
		UPraiseGameInstance* PraiseGameInstance = Cast<UPraiseGameInstance>(GetGameInstance());
		
		if(PraiseGameInstance->Implements<UMenuInterface>())
			PlayerMenuInterface = PraiseGameInstance;

		if (PraiseGameInstance->Implements<UPlayerHUDInterface>())
		{
			PlayerHUDInterface = PraiseGameInstance;
			PlayerHUDInterface->SwitchCrosshair(true);
			PlayerHUDInterface->EnablePlayerStatsBar(true, this);
		}
	}

	if (GetPraiseAnimInstance()) 
	{
		UPraiseAnimInstance* PraiseAnimInstance = GetPraiseAnimInstance();
		PraiseAnimInstance->OnStateUpdate.BindUFunction(this, FName("OnAnimStateUpdate"));
	}
	HandleSprint();
	MouseRot = GetActorRotation();
}
// END SETUP

// GETTERS
float APraisePlayerCharacter::GetCharToCameraDegrees() const
{
	FVector CharFwd = GetActorForwardVector();
	FVector CamFwd = MouseRot.Vector().GetSafeNormal();

	CharFwd.Z = 0;
	CamFwd.Z = 0;

	float Degs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CharFwd, CamFwd)));

	FVector CharToCamVector = ((PlayerCamera->GetComponentLocation() - (PlayerCamera->GetRightVector() * GetClampedcameraOffset())) - GetPawnViewLocation()).GetSafeNormal();

	FVector Cross = FVector::CrossProduct(GetActorForwardVector(), CharToCamVector);

	if (Degs > 180)
		Degs = FMath::Abs(180 - Degs);

	return Degs * -FMath::Sign(Cross.Z);
}

void APraisePlayerCharacter::GetPlayerViewPoint(FVector& OutLocation, FRotator& OutRotation, bool bIsCameraView) const
{
	OutLocation = bIsCameraView ? PlayerCamera->GetComponentLocation() : GetPawnViewLocation();
	OutRotation = GetViewRotation();
}

bool APraisePlayerCharacter::IsTargetInCameraFOV(AActor* Target, bool bCheckCol)
{
	FVector VectorTo = Target->GetActorLocation() - PlayerCamera->GetComponentLocation();

	FVector Dir = VectorTo.GetSafeNormal();

	if (bCheckCol)
		if (!CanSeeTarget(Target))
			return false;

	float Rads = FMath::Acos(FVector::DotProduct(PlayerCamera->GetForwardVector(), Dir));

	return FMath::RadiansToDegrees(Rads) <= 90;
}
void APraisePlayerCharacter::OnFistBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;

	if (!IsWeaponEquiped() || CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty()) return;

	if (OtherActor->Implements<UDamageable>())
	{
		AFist* CharFist = Cast<AFist>(CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon());
		Cast<IDamageable>(OtherActor)->GetWeaponDamage(-CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponDamage(), EAttackType::PRIMARY_ATTACK, EWeaponSlot::PRIMARY_WEAPON, this, CharFist);
	}

	PlayCombatSFX(ECombatSFX::WEAPON_HIT, FUtilities::GetCombatSFXParamName(ECombatSFX::WEAPON_HIT), DEFAULT_UNARMED_HIT_SFX);
}
void APraisePlayerCharacter::OnKickBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;

	if (!IsWeaponEquiped() || GetCurrentMainWeapon()->IsEmpty()) return;

	if (OtherActor->Implements<UDamageable>())
	{
		if (OtherActor->Implements<UCombatible>() && !Cast<ICombatible>(OtherActor)->IsBlocking())
		{
			Cast<IDamageable>(OtherActor)->GetDamage(-10.f, this, EAttackType::KICK);
		}
		else Cast<IDamageable>(OtherActor)->GetDamage(-10.f, this, EAttackType::KICK);
	}

	PlayCombatSFX(ECombatSFX::WEAPON_HIT, FUtilities::GetCombatSFXParamName(ECombatSFX::WEAPON_HIT), DEFAULT_UNARMED_HIT_SFX);
}
// END GETTERS

void APraisePlayerCharacter::SetCharacterDead(AActor* Killer)
{
	
	if (HandCollider_R)
		HandCollider_R->OnComponentBeginOverlap.RemoveDynamic(this, &APraisePlayerCharacter::OnFistBeginOverlap);
	if (HandCollider_L)
		HandCollider_L->OnComponentBeginOverlap.RemoveDynamic(this, &APraisePlayerCharacter::OnFistBeginOverlap);
	if (KickCollider_R)
		KickCollider_R->OnComponentBeginOverlap.RemoveDynamic(this, &APraisePlayerCharacter::OnKickBeginOverlap);
	if (KickCollider_L)
		KickCollider_L->OnComponentBeginOverlap.RemoveDynamic(this, &APraisePlayerCharacter::OnKickBeginOverlap);

	EnableHandCollider(false, true);
	EnableHandCollider(false, false);
	EnableKickCollider(false, true);
	EnableKickCollider(false, false);

	if (HasWeapon())
	{
		for (int i = 0; i < CharWeaponSlots.Num(); i++)
		{
			if (!CharWeaponSlots[(EWeaponSlot)i]->IsEmpty())
			{
				CharWeaponSlots[(EWeaponSlot)i]->ClearSlot()->OnWeaponDropped();
			}

		}
	}

	if (PlayerHUDInterface)
	{
		PlayerHUDInterface->SwitchCrosshair(false);
		PlayerHUDInterface->EnablePlayerStatsBar(false, this, true);
	}

	Super::SetCharacterDead(Killer);
}

void APraisePlayerCharacter::DestroyCharacter()
{
	Super::DestroyCharacter();

	if (PlayerMenuInterface)
	{
		PlayerMenuInterface->LoadMenu(EGameMenu::ENDGAME);
	}
}

void APraisePlayerCharacter::HandleMessage(FTelegram& Msg)
{
	Super::HandleMessage(Msg);
}

float APraisePlayerCharacter::Sine(float Time, float Freq, float Amplitude, float Phase)
{
	float AngularFreq = 2 * PI * Freq;
	float Omega = AngularFreq * Time;

	return Amplitude * FMath::Sin(Omega + (Phase * ((PI * 2) / 360)));
}
// TICK
void APraisePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHandleSpeedChange)
		SpeedChangeTL.TickTimeline(DeltaTime);

	switch (bIsHoldingPrimary) 
	{
		case(true):
			if (bIsAttacking && IsWeaponEquiped() && CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON) && CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->HoldsA<AFirearm>() && bIsAiming)
				if (CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEquiped())
					CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->Attack();
			break;
		case(false):
			break;
	}

	switch (bIsHoldingSecondary) 
	{
		case(true):
			if (IsWeaponEquiped() && CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON) && CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->HoldsA<AFirearm>()) 
			{
				if (CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEquiped()) 
				{
					HandleZoom(DeltaTime, bIsZoomRequested);

					float Dir = GetDirectionDegrees();
					
					ECharVector DirVector = GetDirectionVector(Dir);

					FRotator TargetRot = FRotator();

					if (GetPraiseAnimInstance()->GetAimOffsets(bIsCrouched).Contains(DirVector))
					{
						TargetRot = GetPraiseAnimInstance()->GetAimOffsets(bIsCrouched)[DirVector];

						if (GetVelocity().Size() > 0)
						{
							if (DirVector == ECharVector::FWD)
							{
								if (!bIsCrouched)
									TargetRot += FRotator(0, -5, 20);

								else TargetRot += FRotator(0, 0, 10);
							}		
						}
					}

					FVector ToCam = (PlayerCamera->GetComponentLocation() - (GetActorRightVector() * GetClampedcameraOffset())) - GetPawnViewLocation();

					FVector ToCamInv = ToCam * -1;

					float Degs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToCam)));

					ToCamInv.Z = 0;
					float Yaw = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToCamInv.GetSafeNormal())));
					float Pitch = FMath::RadiansToDegrees(FMath::Asin(FVector::DotProduct(ToCam.GetSafeNormal(), GetActorUpVector())));

					FVector Cross = FVector::CrossProduct(GetActorForwardVector(), ToCamInv.GetSafeNormal());

					if (bIsCrouched)
						Pitch = Pitch - 23;

					FRotator Offset = FRotator(0, FMath::Clamp<int>(Pitch, -45, 45), 0);

					TargetRot += Offset;

					AimRotOffset = FMath::RInterpConstantTo(AimRotOffset, TargetRot, DeltaTime, 100);

					HandleRotateToView(DeltaTime);
				}
			}
			break;
		case(false):
			if (!bIsZoomRequested)
				HandleZoom(DeltaTime, bIsZoomRequested);
			
			break;
	}

	if (bIsReloading || bIsRunning)
		bIsZoomRequested = false;

	if (bIsAttacking && !bIsTargeting)
		HandleRotateToView(DeltaTime);

	if (bIsPressingWeaponSlotKey)
		SecondsPressingWeaponKey += DeltaTime;

	if (SecondsPressingWeaponKey >= SecondsToDropWeapon && CanExecuteAction(ECharAction::DROP_WEAPON) && bIsPressingWeaponSlotKey)
	{
		bIsDroppingWeapon = true;
		bIsPressingWeaponSlotKey = false;

		if (PressedWeaponSlot != EWeaponSlot::INVALID)
		{
			DropWeapon(PressedWeaponSlot);
		}
	}

}

void APraisePlayerCharacter::HandleZoom(float DeltaTime, bool bZoomIn)
{
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, bZoomIn ? DEFAULT_CAMERA_ZOOM - CameraZoom : DEFAULT_CAMERA_ZOOM, DeltaTime, bZoomIn ? CameraZoomSpeed * 3.f: CameraZoomSpeed);
}

void APraisePlayerCharacter::HandleRotateToView(float DeltaTime)
{
	float Degs = GetCharToCameraDegrees();
	
	if (FMath::Abs(Degs) < 3.f) return;

	FRotator TargetRot = FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw + Degs, GetActorRotation().Roll);

	if (GetVelocity().Size() <= 0)
	{
		FRotator YawRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 7);

		SetActorRotation(YawRot);
	}

	else SetActorRotation(TargetRot);


}

void APraisePlayerCharacter::HandleAimWeaponPosition(float DeltaTime)
{
	if (!IsWeaponEquiped()) return;

	UCharWeaponSlot* FirearmSlot = GetCurrentMainWeapon();

	if (FirearmSlot && FirearmSlot->HoldsA<AFirearm>())
	{
		AFirearm* Weapon = Cast<AFirearm>(FirearmSlot->GetSlotWeapon());

		FVector OriginLoc;
		FVector TargetLoc;
		FRotator OriginRot;
		FRotator TargetRot;
		if (bIsAiming)
		{
			OriginLoc = Weapon->GetTransform().InverseTransformPosition(Weapon->GetActorLocation());			
			OriginRot = Weapon->GetEquipRotation();
			TargetLoc = Weapon->GetAimingLocation();
			TargetRot = Weapon->GetAimingRotation();
		}
		else
		{
			OriginLoc = Weapon->GetTransform().InverseTransformPosition(Weapon->GetActorLocation());
			OriginRot = Weapon->GetAimingRotation();
			TargetLoc = Weapon->GetEquipLocation();
			TargetRot = Weapon->GetEquipRotation();

		}
		

		FVector Loc = FMath::VInterpTo(OriginLoc, TargetLoc, DeltaTime, 5.f);

		FRotator Rot = FMath::RInterpTo(OriginRot, TargetRot, DeltaTime, 5.f);

		Weapon->SetActorRelativeRotation(Rot);
		Weapon->SetActorRelativeLocation(Loc);
	}
	
}

void APraisePlayerCharacter::TrySkipTarget_L()
{
	TrySkipTarget(/*bSkipLeft:*/ true);
}

void APraisePlayerCharacter::TrySkipTarget_R()
{
	TrySkipTarget(/*bSkipLeft:*/ false);
}

void APraisePlayerCharacter::OnAnimStateUpdate(ECharAnimState State, bool bIsEnabled)
{
	Super::OnAnimStateUpdate(State, bIsEnabled);

	switch (State)
	{
	case(ECharAnimState::IDLE):
		
		break;

	case(ECharAnimState::EQUIPING):
		
		if (bIsSwitchingWeapons) break;

		if (CharStats) 
		{
			if (IsWeaponEquiped())
				UpdateWalkSpeed(ECurveType::ANTILOG);
			
			else RestoreWalkSpeed(true, ECurveType::ANTILOG);
		}

		if (!IsWeaponEquiped())
		{
			if (TargetingComponent->IsTargetLocked())
			{
				TargetingComponent->ClearTarget();
				bIsTargeting = false;
				GetCharacterMovement()->bOrientRotationToMovement = true;
			}
		}
			
		break;
	case(ECharAnimState::ATTACKING):

		break;
	case(ECharAnimState::JUMPING):
		bIsJumping = bIsEnabled;
		break;
	case(ECharAnimState::ROLLING):
		bIsRolling = bIsEnabled;
		break;
	case(ECharAnimState::RELOADING):
		if (bIsAiming)
			bIsZoomRequested = true;
		break;
	case(ECharAnimState::EVADING):

		bIsEvading = bIsEnabled;

		if (!bIsEvading)
			EvadeDirection = ECharVector::NONE;

		break;
	}
}
void APraisePlayerCharacter::EnableHandCollider(bool bEnable, bool bIsRightHand)
{
	if (!IsWeaponEquiped()) return;

	FName ColProfileName = bEnable ? FName("Weapon") : FName("NoCollision");
	ECollisionEnabled::Type ColType = bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	
	switch (bIsRightHand) {
		case(true):
			if (!HandCollider_R) return;
			HandCollider_R->SetCollisionProfileName(ColProfileName);
			HandCollider_R->SetCollisionEnabled(ColType);
			HandCollider_R->SetActive(bEnable);

			break;
		case(false):
			if (!HandCollider_L) return;
			HandCollider_L->SetCollisionProfileName(ColProfileName);
			HandCollider_L->SetCollisionEnabled(ColType);
			HandCollider_L->SetActive(bEnable);
			break;
	}

	if (bIsAttacking && bEnable)
	{
		PlayCombatSFX(ECombatSFX::WEAPON_ATTACK, FUtilities::GetCombatSFXParamName(ECombatSFX::WEAPON_ATTACK), FMath::RandRange(0, 4));
		PlayCharSFX(ECharSFX::CHAR_ATTACK);
	}
		
}

void APraisePlayerCharacter::EnableKickCollider(bool bEnable, bool bIsRightLeg)
{
	if (!IsWeaponEquiped()) return; 
	FName ColProfileName = bEnable ? FName("Weapon") : FName("NoCollision");
	ECollisionEnabled::Type ColType = bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	switch (bIsRightLeg)
	{
	case(true):
		if (!KickCollider_R) return;
		KickCollider_R->SetCollisionProfileName(ColProfileName);
		KickCollider_R->SetCollisionEnabled(ColType);
		KickCollider_R->SetActive(bEnable);
		break;
	case(false):
		if (!KickCollider_L) return;
		KickCollider_L->SetCollisionProfileName(ColProfileName);
		KickCollider_L->SetCollisionEnabled(ColType);
		KickCollider_L->SetActive(bEnable);
		break;
	}
	
	if (bIsAttacking && bEnable)
	{
		PlayCombatSFX(ECombatSFX::WEAPON_ATTACK, FUtilities::GetCombatSFXParamName(ECombatSFX::WEAPON_ATTACK), FMath::RandRange(0, 4));
		PlayCharSFX(ECharSFX::CHAR_ATTACK);
	}
}

void APraisePlayerCharacter::OnHandColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& HitResult)
{
	FLogger::LogTrace(__FUNCTION__ + FString(" :: ON PLAYER COLLIDER HIT"));
}
void APraisePlayerCharacter::OnInteractionBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AWeapon>())
	{
		//TODO: EnableStencil()
	}
}

void APraisePlayerCharacter::OnInteractionEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<AWeapon>())
	{
		//TODO
	}
}


void APraisePlayerCharacter::MoveForward(float Value)
{
	if (!bCanMove) return;

	if (!Controller || Value == 0.f) return;

	FRotator Direction = Controller->GetControlRotation();

	const FRotator YawRotation = FRotator(0.f, Direction.Yaw, 0.f);
	
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
}

void APraisePlayerCharacter::MoveRight(float Value)	
{
	if (!bCanMove) return;

	if (!Controller || Value == 0.f) return;

	FRotator Direction = Controller->GetControlRotation();
	
	const FRotator PitchRotation = FRotator(0, Direction.Yaw, 0);

	AddMovementInput(FRotationMatrix(PitchRotation).GetUnitAxis(EAxis::Y), Value);
}

void APraisePlayerCharacter::AddMouseVerticalInput(float Value)
{
	Super::AddControllerPitchInput(Value);
}

void APraisePlayerCharacter::AddMouseHorizontalInput(float Value)
{
	Super::AddControllerYawInput(Value);

	FRotator DeltaRot(MouseRot);

	FRotator ViewRot = GetViewRotation();

	MouseRot.Yaw = ViewRot.Yaw;
}

void APraisePlayerCharacter::TriggerEquipWeapons()
{
	if (bIsEquiping) return;

	if (bIsPressingPickupKey) return;

	if (!HasWeapon()) return;

	if (IsWeaponEquiped()) 
	{
		if (!bIsSwitchingWeapons && bIsTargeting)
		{
			TargetingComponent->ClearTarget();
			bIsTargeting = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		
	}

	EquipWeapons();
}

void APraisePlayerCharacter::HandleInGameMenu()
{
	if (!PlayerMenuInterface) return;

	if (bInGameMenuEnabled)
	{
		bInGameMenuEnabled = false;
		PlayerMenuInterface->TeardownMenu(EGameMenu::INGAME);
	}
	else
	{
		bInGameMenuEnabled = true;

		PlayerMenuInterface->LoadMenu(EGameMenu::INGAME);
	}
}

void APraisePlayerCharacter::EnsureReleaseBlock()
{
	if (bIsBlocking)
		EndBlocking();
}

void APraisePlayerCharacter::TriggerWeaponSlot(EWeaponSlot Slot)
{
	bIsPressingWeaponSlotKey = false;

	if (!bIsDroppingWeapon)
	{
		if (!bIsDroppingWeapon && SecondsPressingWeaponKey < SecondsToDropWeapon && !bIsEquiping)
			TrySwitchWeapon(Slot);

		PressedWeaponSlot = EWeaponSlot::INVALID;
	}
		

	SecondsPressingWeaponKey = 0;
	bIsDroppingWeapon = false;
}


void APraisePlayerCharacter::DropWeaponSlot(EWeaponSlot Slot)
{
	PressedWeaponSlot = Slot;
	
	bIsPressingWeaponSlotKey = true;
}

void APraisePlayerCharacter::StartJump()
{
	if (IsWeaponEquiped())
	{
		if (bIsTargeting || bIsBlocking)
		{
			if (!CanExecuteAction(ECharAction::EVADE)) return;

			TriggerEvade();

			return;
		}
		else
		{
			if (!CanExecuteAction(ECharAction::JUMP)) return;

			Super::StartJump();

			HandleActionStaminaDecrease(ECharAction::JUMP);
			
			PlayCharSFX(ECharSFX::CHAR_EVADE);
		}
	}
	else
	{
		if (!CanExecuteAction(ECharAction::JUMP)) return;

		Super::StartJump();

		HandleActionStaminaDecrease(ECharAction::JUMP);

		PlayCharSFX(ECharSFX::CHAR_EVADE);
	}
}

void APraisePlayerCharacter::TriggerEvade()
{
	if (!CanExecuteAction(ECharAction::EVADE)) return;

	EvadeDirection = GetCharMovementVector();

	if (EvadeDirection == ECharVector::NONE)
		EvadeDirection = ECharVector::BWD;

	bIsEvading = true;

	PlayEvadeAnimation(EvadeDirection);
}

void APraisePlayerCharacter::EndJump()
{
	Super::EndJump();
}

void APraisePlayerCharacter::HandleSpeedChange(float DeltaTime)
{
	SpeedChangeTL.TickTimeline(DeltaTime);
}

void APraisePlayerCharacter::StartCrouch()
{
	Super::StartCrouch();

	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
}

void APraisePlayerCharacter::EndCrouch()
{
	Super::EndCrouch();
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
}


// --------- Input - Combat System

void APraisePlayerCharacter::EquipWeapons(EWeaponSlot Slot, bool bTryDualEquip)
{
	if (!CanExecuteAction(ECharAction::EQUIP_WEAPON)) return;

	if (bIsPressingWeaponSlotKey) return;

	EquippingHand = -1;

	EWeaponSlot SecondSlot = Slot == EWeaponSlot::INVALID ? EWeaponSlot::INVALID : Slot == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

	AWeapon* PrimaryWeapon = nullptr;
	AWeapon* SecondaryWeapon = nullptr;
	
	if (CharWeaponSlots.Num() <= 0) return;

	if (IsWeaponEquiped())
	{
		if (Slot == EWeaponSlot::INVALID)
		{
			Slot = GetCurrentMainWeapon()->GetEquipedHand();
			EWeaponSlot SecondHand = Slot == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

			SecondSlot = CurrentWeapons.Contains(SecondHand) && !CurrentWeapons[SecondHand]->IsEmpty() ? SecondHand : EWeaponSlot::INVALID;
		}

		if (CurrentWeapons.Contains(Slot))
		{
			if (!CurrentWeapons[Slot]->IsEmpty() && CurrentWeapons[Slot]->IsEquiped())
			{
				if (!CurrentWeapons[Slot]->GetSlotWeapon()->IsTwoHand()) 
				{
					if (SecondSlot != EWeaponSlot::INVALID && CurrentWeapons.Contains(SecondSlot) && CurrentWeapons[SecondSlot]->HoldsA<AWeapon>() && !CurrentWeapons[SecondSlot]->GetSlotWeapon()->IsTwoHand())
					{
						SecondaryWeapon = CurrentWeapons[SecondSlot]->GetSlotWeapon();
			
						EquipSlotWeapon(CurrentWeapons[Slot],
							/*bIsEquip*/ false,
							/*EquipHand*/ CurrentWeapons[Slot]->GetHolsterEquippingHand(),
							/*bIsDualEquip*/ CurrentWeapons[Slot]->HoldsA<AFist>() && (CurrentWeapons[SecondSlot]->HoldsA<AFist>() || CurrentWeapons[SecondSlot]->IsEmpty()) ? false : bTryDualEquip,
							/*SecondaryWeapon*/  CurrentWeapons[Slot]->HoldsA<AFist>() && (CurrentWeapons[SecondSlot]->HoldsA<AFist>() || CurrentWeapons[SecondSlot]->IsEmpty()) ? nullptr : CurrentWeapons[SecondSlot],
							/*bShouldLinkAnims:*/ CurrentWeapons[Slot]->HoldsA<AFist>() && (CurrentWeapons[SecondSlot]->HoldsA<AFist>() || CurrentWeapons[SecondSlot]->IsEmpty()) ? false : !CurrentWeapons[Slot]->CanShareEquipAnim(CurrentWeapons[SecondSlot]));
						
					}

					else EquipSlotWeapon(CurrentWeapons[Slot], /*bIsEquip*/ false, Slot); 
				}

				else EquipSlotWeapon(CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON], /*bIsEquip*/ false, EWeaponSlot::PRIMARY_WEAPON); 

			}

			else FLogger::LogTrace(__FUNCTION__ + FString("CURRENT SLOT NOT EQUIPED"));
		}

		else FLogger::LogTrace(__FUNCTION__ + FString("CURRENT WEAPONS DOES NOT CONTAIN SLOT"));
	}
	else
	{
		if (Slot == EWeaponSlot::INVALID)
		{
			Slot = EWeaponSlot::PRIMARY_WEAPON;
			SecondSlot = CharWeaponSlots.Contains(EWeaponSlot::SECONDARY_WEAPON) && !CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->IsEmpty() ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::INVALID;
		}
	
		if (!CharWeaponSlots[Slot]->IsEmpty()) 
		{
			PrimaryWeapon = CharWeaponSlots[Slot]->GetSlotWeapon();
			
			if (PrimaryWeapon->IsTwoHand())
			{
				EquipSlotWeapon(CharWeaponSlots[Slot], true, EWeaponSlot::PRIMARY_WEAPON);
				return;
			}
			else 
			{
				if (SecondSlot != EWeaponSlot::INVALID && !CharWeaponSlots[SecondSlot]->IsEmpty()) 
				{
					SecondaryWeapon = CharWeaponSlots[SecondSlot]->GetSlotWeapon();
					
					if (!SecondaryWeapon->IsTwoHand())
					{
						if ((CharWeaponSlots[Slot]->GetHolsterEquippingHand() != CharWeaponSlots[SecondSlot]->GetHolsterEquippingHand()) && bTryDualEquip)
						{
							EquipSlotWeapon(CharWeaponSlots[Slot],
								/*bIsEquip*/ true,
								/*EquipHand*/ CharWeaponSlots[Slot]->GetHolsterEquippingHand(),
								/*bIsDualEquip*/  bTryDualEquip,
								/*SecondaryWeapon*/ CharWeaponSlots[SecondSlot],
								/*bShouldLinkAnims*/!CharWeaponSlots[Slot]->CanShareEquipAnim(CharWeaponSlots[SecondSlot]));
						}

						else EquipSlotWeapon(CharWeaponSlots[Slot], true, CharWeaponSlots[Slot]->GetHolsterEquippingHand());
					}

					else EquipSlotWeapon(CharWeaponSlots[Slot], true, CharWeaponSlots[Slot]->GetHolsterEquippingHand(), false);
				}

				else EquipSlotWeapon(CharWeaponSlots[Slot], true, CharWeaponSlots[Slot]->GetHolsterEquippingHand());
			}			
		}
	}
}

void APraisePlayerCharacter::StartPrimaryAttack()
{
	bIsHoldingPrimary = true;

	if (!IsWeaponEquiped()) return;

	RequestedAttackType = EAttackType::PRIMARY_ATTACK;
	
	UCharWeaponSlot* MainWeaponSlot = GetCurrentMainWeapon();

	if (!MainWeaponSlot) return;

	if (MainWeaponSlot->IsEmpty()) return;

	if (MainWeaponSlot->GetSlotWeapon()->GetIsBroken())
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" Weapon is broken"), FColor::Red);

		return;
	}

	if (!CanExecuteAction(ECharAction::ATTACK)) return;
	
	if (bCanAttack) 
		HandleAnimatorAttackRequest(MainWeaponSlot); 
	
	HandleComboStuff(MainWeaponSlot->GetSlotWeapon());
	
}

void APraisePlayerCharacter::EndPrimaryAttack()
{
	bIsHoldingPrimary = false;

	if (CurrentPose == EArmedPoses::RIFLE && IsWeaponEquiped() && bIsAiming) 
	{
		bCanAttack = true;
		bIsAttacking = false;
		CurrentAttackType = EAttackType::NONE;
	}		
}

void APraisePlayerCharacter::StartSecondaryAttack()
{
	bIsHoldingSecondary = true;
	if (CurrentPose == EArmedPoses::RIFLE) 
	{
		bIsAiming = true;					
		bIsZoomRequested = true;			
		GetCharacterMovement()->bOrientRotationToMovement = false;
		EndSprint();
	}
	else 
	{
		if (!bCanAttack) return;

		if (!IsWeaponEquiped() || bIsEquiping) return;
		
		UCharWeaponSlot* WeaponSlot = CurrentWeapons.Num() == 1 ? GetCurrentMainWeapon() : GetCurrentSecondaryWeapon();
		
		if (!WeaponSlot) return;

		if (WeaponSlot->GetSlotWeapon()->GetIsBroken())
		{
			FLogger::LogTrace(__FUNCTION__ + FString(" Weapon is broken"), FColor::Red);
			return;
		}
		
		RequestedAttackType = EAttackType::SECONDARY_ATTACK;

		if (!CanExecuteAction(ECharAction::ATTACK)) return;

		CurrentAttackWeapon = WeaponSlot->GetSlotWeapon();
		CurrentAttackType = EAttackType::SECONDARY_ATTACK;
		bIsAttacking = true;
		bCanAttack = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		HandleActionStaminaDecrease(ECharAction::ATTACK);
	}
}

void APraisePlayerCharacter::EndSecondaryAttack()
{
	bIsHoldingSecondary = false;

	if (CurrentPose == EArmedPoses::RIFLE) 
	{
		bIsAiming = false; 
		bIsZoomRequested = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}	
}

void APraisePlayerCharacter::StartBlocking()
{
	if (!CanExecuteAction(ECharAction::BLOCK)) return;

	if (!IsWeaponEquiped()) return;

	if(!bIsBlocking)
		bIsBlocking = true;

	bIsZoomRequested = false;

	bIsHoldingSecondary = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	UCharWeaponSlot* MainSlot = GetCurrentMainWeapon();

	if (!MainSlot) return;

	if (MainSlot->HoldsA<AFirearm>() && MainSlot->IsEquiped()) 
	{
		MainSlot->GetSlotWeapon()->SetActorRelativeRotation(FRotator(76.2, 172.8, 187.2));
		MainSlot->GetSlotWeapon()->SetActorRelativeLocation(FVector(4, 3, 8));
	}
}

void APraisePlayerCharacter::EndBlocking()
{
	bIsBlocking = false;
	bIsHoldingSecondary = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	UCharWeaponSlot* MainSlot = GetCurrentMainWeapon();

	if (!MainSlot) return;
	
	ResetMainWeaponEquipPosition();
}

void APraisePlayerCharacter::StartParry()
{
	if (!IsWeaponEquiped() || bIsEquiping || bIsParring) return;

	UCharWeaponSlot* WeaponSlot = CurrentWeapons.Num() == 1 ? GetCurrentMainWeapon() : GetCurrentSecondaryWeapon();

	if (!WeaponSlot) return;

	if (WeaponSlot->GetSlotWeapon()->GetIsBroken()) return;

	RequestedAttackType = EAttackType::PARRY;

	if (!CanExecuteAction(ECharAction::PARRY)) return;
	
	CurrentAttackWeapon = WeaponSlot->GetSlotWeapon();
	CurrentAttackType = EAttackType::PARRY;
	bIsParring = true;
	bCanAttack = false;

	HandleActionStaminaDecrease(ECharAction::PARRY);
}

void APraisePlayerCharacter::EndParry()
{
}

void APraisePlayerCharacter::TryLockTarget()
{
	if (!TargetingComponent) return;

	if (TargetingComponent->IsTargetLocked()) 
	{
		TargetingComponent->GetCurrentTarget()->EnableTargetWidget(false);
		TargetingComponent->ClearTarget();
		bIsTargeting = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		return;
	}
	
	if (IsWeaponEquiped()) 
	{
		if (TargetingComponent->TryLockTarget(true))
		{
			bIsTargeting = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			TargetingComponent->GetCurrentTarget()->EnableTargetWidget(true);
		}
	}
}
void APraisePlayerCharacter::TrySkipTarget(bool bSkipLeft)
{
	if (!TargetingComponent) return;

	if (!bIsTargeting) return;

	if (!TargetingComponent->IsTargetLocked()) return;

	TargetingComponent->TrySkipTarget(bSkipLeft);
}


void APraisePlayerCharacter::OnHoldedAttackHandler(EAttackType AttackType)
{
	switch (AttackType) {
	case(EAttackType::PRIMARY_ATTACK):
		if (CurrentWeapons.Num() > 0 &&
			CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON] &&
			!CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty() &&
			CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEquiped()) {
			
			if (CurrentPose == EArmedPoses::RIFLE) 
				bIsAttacking = false;
		}
		break;
	case(EAttackType::SECONDARY_ATTACK):
		break;
	default:
		break;
	}
}



