#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include "DelimitedString.h"
#include "../../Enums/Animation/EHolsterPosition.h"
#include "../../Enums/CommonUtility/ECharVector.h"
#include "../../Enums/CombatSystem/ECombatSFX.h"
#include "../../Enums/CommonUtility/EMiscSFX.h"
#include "../../Enums/Characters/ECharSFX.h"
#include "FLogger.h"
#include "FUtilities.generated.h"

USTRUCT()
struct PRAISE_API FUtilities
{
	GENERATED_BODY()


	/// <summary>
	/// https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
	/// Method 1.3
	/// </summary>
	/// <param name="sentence"></param>
	/// <param name="delimiter"></param>
	/// <returns></returns>
	static std::vector<std::string> Split(const std::string sentence, char delimiter)
	{
		std::vector<std::string> tokens;

		std::string currentToken;

		std::istringstream tokenStream(sentence);

		while (std::getline(tokenStream, currentToken, delimiter))
		{
			tokens.push_back(currentToken);
		}

		return tokens;
	}

	static TArray<FString> Split(const FString sentence, char delimiter)
	{
		TArray<FString> results;

		std::vector<std::string> tokens;

		std::string currentToken;

		std::istringstream tokenStream(FUtilities::FStringToStdString(sentence));

		while (std::getline(tokenStream, currentToken, delimiter))
		{
			results.Add(FUtilities::StdStringToFString(currentToken));
		}

		return results;
	}

	/// <summary>
	/// https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/ 
	/// Methods 1.1 and 1.2
	/// </summary>
	/// <param name="sentence"></param>
	/// <returns></returns>
	static std::vector<std::string> SplitCommas(const std::string sentence)
	{
		std::istringstream iss(sentence);
		std::vector<std::string> results(std::istream_iterator<DelimitedString<','>>{iss}, std::istream_iterator<DelimitedString<','>>());
		return results;
	}

	static std::vector<std::string> SplitDash(const std::string sentence)
	{
		std::istringstream iss(sentence);
		std::vector<std::string> results(std::istream_iterator<DelimitedString<'-'>>{iss}, std::istream_iterator<DelimitedString<'-'>>());
		return results;
	}

	static std::string FStringToStdString(FString FString)
	{
		return std::string(TCHAR_TO_UTF8(*FString));
	}

	static FString StdStringToFString(std::string String)
	{
		return FString(String.c_str());
	}

	static const FString EnumToString(const TCHAR* Enum, int32 EnumValue)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
		
		if (!EnumPtr)
			return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
		
		return EnumPtr->GetNameStringByIndex(EnumValue);
	}

	static const int64 StringToEnumValue(const TCHAR* Enum, FString EnumValue)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);

		if (!EnumPtr)
			return -1;

		return EnumPtr->GetValueByNameString(EnumValue);
	}

	static const int64 GetEnumCount(const TCHAR* Enum)
	{
		const UEnum* m_enum = FindObject<UEnum>(ANY_PACKAGE, Enum, true);

		return m_enum->GetMaxEnumValue();
	}

	static const FName GetSocketName(EHolsterPosition HolsterPosition)
	{
		switch (HolsterPosition) {
			case(EHolsterPosition::UNARMED):
				return FName("ik_hand_gun");
			case(EHolsterPosition::SHOULDER_R):
				return FName("Shoulder_R_BSocket");
			case(EHolsterPosition::LOW_BACK_L):
				return FName("Back_L_Socket");
			case(EHolsterPosition::LOW_BACK_R):
				return FName("Back_R_Socket");
			case(EHolsterPosition::THIGH_R):
				return FName("Thigh_R_Socket");
			case(EHolsterPosition::THIGH_L):
				return FName("Thigh_L_Socket");
			case(EHolsterPosition::CHEST_L):
				return FName("");
			case(EHolsterPosition::LOW_CHEST_L):
				return FName("");
			case(EHolsterPosition::HOLSTER_R):
				return FName("Holster_R_Socket");
			default: return FName();
		}
	}

	static const ECharVector GetOpposedCharVector(ECharVector Vector)
	{
		switch (Vector)
		{
			case(ECharVector::FWD):
				return ECharVector::BWD;
			case(ECharVector::FWD_L):
				return ECharVector::BWD_R;
			case(ECharVector::FWD_R):
				return ECharVector::BWD_L;
			case(ECharVector::LEFT):
				return ECharVector::RIGHT;
			case(ECharVector::RIGHT):
				return ECharVector::LEFT;
			case(ECharVector::BWD):
				return ECharVector::FWD;
			case(ECharVector::BWD_L):
				return ECharVector::FWD_R;
			case(ECharVector::BWD_R):
				return ECharVector::FWD_L;
			default:
				return ECharVector::NONE;
		}
	}
	static const bool IsValidVector(const FVector& Vector)
	{
		return !((Vector == FVector::ZeroVector ) || 
			(FMath::Abs(Vector.X) == 2147483648 && FMath::Abs(Vector.Y) == 2147483648 && FMath::Abs(Vector.Z) == 2147483648) || 
			(FMath::Abs(Vector.X) == 3.40282347e+38 && FMath::Abs(Vector.Y) == 3.40282347e+38 && FMath::Abs(Vector.Z) == 3.40282347e+38));
	}

	static const FName GetCombatSFXParamName(ECombatSFX FX)
	{
		switch (FX)
		{
			case(ECombatSFX::WEAPON_EQUIP):
				return FName("EquipSample");
			case(ECombatSFX::WEAPON_UNEQUIP):
				return FName("UnequipSample");
			case(ECombatSFX::WEAPON_DROP):
				return FName("DropSample");
			case(ECombatSFX::WEAPON_PICK):
				return FName("PickSample");
			case(ECombatSFX::WEAPON_BLOCK):
				return FName("BlockSample");
			case(ECombatSFX::WEAPON_PARRY):
				return FName("ParrySample");
			case(ECombatSFX::WEAPON_HIT):
				return FName("AttackHitSample");
			case(ECombatSFX::WEAPON_ATTACK):
				return FName("AttackSample");
			case(ECombatSFX::WEAPON_BROKEN):
			default:
				return FName();
		}
		return FName();
	}

	static const FName GetCharSFXParamName(ECharSFX FX)
	{
		switch (FX)
		{
		case(ECharSFX::CHAR_ATTACK):
			return FName("CharAttackSample");
		case(ECharSFX::CHAR_BLOCK):
			return FName("CharBlockSample");
		case(ECharSFX::CHAR_DAMAGED):
			return FName("CharDamagedSample");
		case(ECharSFX::CHAR_DEAD):
			return FName("CharDeadSample");
		case(ECharSFX::CHAR_EVADE):
			return FName("CharEvadeSample");
		case(ECharSFX::CHAR_TAUNT):
			return FName("CharTauntSample");
		case(ECharSFX::CHAR_BACKUP_REQ):
			return FName("CharBackupSample");
		default:
			return FName();
		}
		return FName();
	}
	static const FName GetMiscSFXParamName(EMiscSFX FX)
	{
		switch (FX)
		{
		case(EMiscSFX::FOOTSTEPS):
			return FName("FootstepSample");
		
		default:
			return FName();
		}
		return FName();
	}
	//
};
