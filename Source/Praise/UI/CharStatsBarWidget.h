// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interfaces/UI/CharStatsInterface.h"
#include "Blueprint/UserWidget.h"

#include "CharStatsBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UCharStatsBarWidget : public UUserWidget, public ICharStatsInterface
{
	GENERATED_BODY()
	
public:
	UCharStatsBarWidget(const FObjectInitializer& ObjectInitializer);
	void SetWidgetOwner(ICharStatsInterface* Owner);
	void ClearWidgetOwner();
private:

	UFUNCTION(BlueprintCallable)
		virtual float GetHealthRatio() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetStaminaRatio() const;

	ICharStatsInterface* WidgetOwner;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	bool bDidInit;
};
