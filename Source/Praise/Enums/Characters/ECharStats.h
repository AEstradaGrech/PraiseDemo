#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECharStats.generated.h"

UENUM(BlueprintType)
enum class ECharStats : uint8
{
	NONE				UMETA(DisplayName = "None"),
	STRENGHT			UMETA(DisplayName = "Strenght"),	    // daño total, disminuye influencia peso arma (velAtak, velWalkRun), capacidad carga?
	CONSTITUTION		UMETA(DisplayName = "Constitution"),	// salud total, stamina, lo rapido que se gasta la stamina (influye block), absorcion daño, capacidad carga?
	INTELLIGENCE		UMETA(DisplayName = "Intelligence"),    // mana y incremento exp. (bonificacion duracion racial? porque es muy listo y controla...)
	DEXTRITY			UMETA(DisplayName = "Dextrity"),		// afecta a roll, velocidad walk/run (+), velAtk, velSeath, velCubrirse, span chanceFatality, dañoCrit
	CHARISMA			UMETA(DisplayName = "Charisma"),		// precio? algo en plan COOP?
	WISDOM				UMETA(DisplayName = "Wisdom"),
	LUCK				UMETA(DisplayName = "Luck"),			//Afecta a prob critico y probabilidad de esquivar porque si o de que un NPC ignore tu agro y ataque a otro
	SPEED				UMETA(DisplayName = "Speed"),
	MAX_SPEED			UMETA(DisplayName = "Max Speed"),
	FOV					UMETA(DisplayName = "Field Of View"),   //afecta a tu capacidad para targetear (al area de deteccion). se puede usar para Ceguera
	LEVEL				UMETA(DisplayName = "Level")
};