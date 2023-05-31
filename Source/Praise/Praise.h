// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define DEFAULT_MAX_HEALTH					float(100.f)
#define DEFAULT_MAX_STAMINA					float(100.f)
#define SPEED_SCALE_FACTOR					int(3)
#define MONTAGE_KEY_BASIC_ATTACK_R			FString("Basic_Attack_R")
#define MONTAGE_KEY_BASIC_ATTACK_L			FString("Basic_Attack_L")
#define MONTAGE_KEY_KICK					FString("Kick")

#define CURVE_PATH_LINEAR				FString("/Game/Core/Curves/Generic/CF_Linear")
#define CURVE_PATH_LOG					FString("/Game/Core/Curves/Generic/CF_Log")
#define CURVE_PATH_ANTILOG				FString("/Game/Core/Curves/Generic/CF_AntiLog")
#define CURVE_PATH_S_LOG				FString("/Game/Core/Curves/Generic/CF_Log_S")
#define CURVE_PATH_S_ANTILOG			FString("/Game/Core/Curves/Generic/CF_AntiLog_S")
#define CURVE_PATH_EQUIP_SHOULDER_1H	FString("/Game/Core/Curves/Animation/Equip/CF_Shoulder_1H")

#define MAX_ATTRIBUTE_POINTS			int32(30)

#define PHYSICS_SURFACE_FLESH_DEFAULT		SurfaceType2
#define PHYSICS_SURFACE_FLESH_VULNERABLE	SurfaceType3
#define PHYSICS_SURFACE_HITTABLE			SurfaceType1

#define STAT_CONFIG						    int32(0) //Overwritef
#define STAT_MODIFIER						int32(1) // Add
#define STAT_MULTIPLIER						int32(2) // Multiply by
#define STAT_PERCENTAGE						int32(3) // Percentage of

#define MIN_STATS_VALUE					int32(-30)
#define MAX_STATS_VALUE					int32(30)
#define MAX_CHAR_LVL_VALUE				int32(100)
#define MAX_WEAPON_LVL_VALUE			int32(20)
#define DEFAULT_HUMAN_ABLTY_GRP			int32(5)
#define DEFAULT_ORC_ABLTY_GRP			int32(6)
#define FIRST_LVL_UP_XP					float(1000.f)		
#define DEFAULT_CHAR_HEALTH				float(100.f)
#define DEFAULT_CHAR_STAMINA			float(100.f)
#define DEFAULT_CHAR_ADRENALINE			float(50.f)
#define DEFAULT_CHAR_MANA			    float(200.f)
#define HEALTH_RESISTANCE_FACTOR		float(0.25f)
#define STAM_RESISTANCE_FACTOR			float(0.5f)
#define DEFAULT_STAM_DECREASE_PERCENT   float(7.f)
#define DEFAULT_CHAR_FOV				float(90.f)
#define DEFAULT_BOT_STAM_MIN_STAM		float(70.f)

#define SKILL_PROGRESSION_ID			uint8(1)
#define ATTRIBUTES_PROGRESSION_ID		uint8(2)

#define DEFAULT_CAMERA_ZOOM				float(300.f)
#define DEFAULT_CAMERA_OFFSET			float(30.f)
#define MAX_CAMERA_OFFSET				float(50.f)
#define MIN_CAMERA_OFFSET				float(-50.f)
#define DEFAULT_COVER_HEIGHT			float(90.f)

// BOT BB KEYS
// Common BB
#define BB_GLOBAL_MAX_IDLE_VAL			float(3600.f)
#define BB_GLOBAL_MAX_WANDERING_VAL		float(60.f)
#define BB_GLOBAL_MAX_CHASING_VAL		float(3600.f)
#define BB_GLOBAL_MAX_RUNAWAY_VAL		float(60.f)
#define BB_GLOBAL_MIN_RUNAWAY_VAL		float(30.f)
#define BB_GLOBAL_MAX_SNITCH_VAL		float(60.f)
#define BB_GLOBAL_MIN_SNITCH_VAL		float(30.f)
#define BB_WANDERING_RADIUS_VAL			float(5000.f)
#define BB_CHASE_TRIGGER_DISTANCE_VAL	float(500.f)
#define BB_GLOBAL_MAX_IDLE				FName("MaxIdleTime")
#define BB_WANDERING_MODE				FName("WanderingMode")
#define BB_WANDERING_RADIUS				FName("WanderingRadius")
#define BB_MAX_WANDERING				FName("MaxWanderingTime")
#define BB_TOTAL_WANDERING				FName("TotalWanderingTime")
#define BB_MAX_CHASING					FName("MaxChasingTime")
#define BB_CHASE_TRIGGER_DISTANCE		FName("ChaseTriggerDistance")
#define BB_CHASING_MODE					FName("ChasingMode")
#define BB_MAX_RUNAWAY					FName("MaxRunawayTime")
#define BB_MIN_RUNAWAY					FName("MinRunawayTime")
#define BB_MAX_SNITCH					FName("MaxSnitchTime")
#define BB_MIN_SNITCH					FName("MinSnitchTime")
#define BB_SUBZONE_PATROL				FName("IsSubZonePatrol")
#define BB_ONLY_SUBZONE_PATROL			FName("OnlySubZonePatrol")
#define BB_WANTS_TO_TRAVEL				FName("WantsToTravel")

#define BB_PRIORITY_TARGET				FName("PriorityTarget")
#define BB_PRIORITY_LOCATION			FName("PriorityLocation")
#define BB_LAST_PRIORITY_LOCATION		FName("LastPriorityLocation")
#define BB_LOCATION_OVER_TARGET			FName("LocationOverTarget")
#define BB_TRAVEL_DESTINATION			FName("TravelDestination")
#define BB_CURRENT_PATH_IS_PARTIAL		FName("CurrentPathIsPartial")
#define BB_CURRENT_WP					FName("CurrentWP")
#define BB_MAX_AGGRO_TARG				FName("MostAggressiveTarget")
#define BB_MAX_DMG_TARG					FName("MostDamagerTarget")
#define BB_LAST_SEEN_TARGET				FName("LastSeenTarget")
#define BB_LAST_DAMAGER_TO_BOT_VECTOR	FName("LastDamagerToBotVector")
#define BB_LAST_KNOWN_TARGET_LOC		FName("LastKnownTargetLocation")
#define BB_LAST_DAMAGER					FName("LastDamager")
#define BB_COVER_SPOT					FName("CoverSpot")
#define BB_SELECT_CLOSEST_FOE			FName("SelectClosestFoE")
#define BB_SPAWN_POINT					FName("SpawnPoint")
#define BB_SPAWN_LOC					FName("SpawnLocation")
#define BB_CAN_TRAVEL					FName("CanTravel")
#define BB_CAN_PATROL					FName("CanPatrol")
#define BB_CAN_WANDER					FName("CanWander")
#define BB_CAN_RUN						FName("CanRun")
#define BB_HAS_BEEN_DAMAGED				FName("HasBeenRecentlyDamaged")
#define BB_HAS_FOLLOW_REQ				FName("HasFollowRequest")

#define BB_ORIGINAL_DIST_TO				FName("OriginalDistanceToTarget")
#define BB_CLEAR_ROUTE_ON_PATROL_ENTER  FName("ClearCurrentRouteOnPatrolEnter")
#define BB_ONEXIT_CLEAR_TARGET			FName("ClearPriorityTargetOnStateExit")
#define BB_IS_PAUSED					FName("IsBrainPaused")
#define BB_DEAD_TARGETS_ALLOWED			FName("DeadTargetsAllowed")
#define BB_ONLY_ENEMY_TARGETS_ALLOWED   FName("OnlyEnemyTargetsAllowed")
#define BB_IS_PATH_BLOCKED				FName("IsPathBlocked")
#define BB_HAS_REACHED_TARGET			FName("HasReachedTarget")
#define BB_HAS_VALID_LOCATION			FName("HasValidTargetLocation")
#define BB_BOT_HOME						FName("BotHome")

//ZombieBB
#define BB_IS_ROARING					FName("IsRoaring")
#define BB_IS_ATTACKING					FName("IsAttacking")
#define BB_CAN_MOVE						FName("CanMove")
#define BB_CAN_ATTACK					FName("CanAttack")
#define BB_CURRENT_TARGET				FName("CurrentTarget")

#define BP_CITIZEN_BOT					FString("BP_CitizenBotCharacter")
#define BP_SEEKER_BOT					FString("BP_PlayerSeekerBotCharacter")
#define BP_GUARD_BOT					FString("BP_GuardBotCharacter")
#define BP_SEEKER_DEMON_BOT				FString("BP_SeekerDemonBotCharacter")
#define BP_BASIC_ZOMBIE_BOT				FString("BP_BasicZombieBotCharacter")

#define DEFAULT_FIREARM_MODEL			FString("AK-47")
#define DEFAULT_BAYONET_RIFLE_MODEL		FString("Default Bayonet Rifle")
#define DEFAULT_PIKE_2H_MODEL			FString("Default Pike2H")
#define DEFAULT_PIKE_1H_MODEL			FString("Default Pike1H")
#define DEFAULT_BLUNT_1H_MODEL			FString("Default Blunt 1H")
#define DEFAULT_MID_SHIELD_MODEL		FString("Default MidShield")
#define DEFAULT_HANDTORCH_MODEL			FString("Default HandTorch")
#define DEFAULT_SHARP_1H_MODEL			FString("Default Sharp 1H")
#define DEFAULT_SWORD_1H_MODEL			FString("Default Sword 1H")
#define DEFAULT_SHORTSWORD_1H_MODEL		FString("Default Short Sword 1H")

#define DEFAULT_ZOMBIE_FIST_CONFIG		int(1)

#define WP_SPAWN_NODE					FString("SpawnNode")
#define WP_CURRENT_ROUTE				FString("CurrentRoute")
#define WP_ZONE_COL_LOC					FString("ZoneColliderLocation")
#define WP_CROSSROAD					FString("Crossroad")
#define WP_MAIN_ZONE					FString("MainZone")
#define WP_SUB_ZONE						FString("SubZoneEntry")
#define WP_VIP							FString("VeryImportantPlace")
#define WP_OUTDOOR						FString("OutdoorPath")
#define WP_INDOOR						FString("IndoorPath")
#define WP_WAITING_SPOT					FString("WaitingSpot")
#define WP_ZONE_SPAWN_POINT				FString("ZoneSpawnPoint")

#define DEFAULT_UNARMED_HIT_SFX			int(7)
#define DEFAULT_UNARMED_CRIT_HIT_SFX	int(8)
#define DEFAULT_FIREARM_EMPTY_SFX		int(5) 

#define MAP_MAIN_MENU					FName("/Game/Core/Maps/MainMenu")
#define MAP_BOTVILLE					FName("/Game/Core/Maps/Alvaro/BotVille_v1")

#define BOT_DEFAULT_BB					FName("BB_CommonBlackboard")

