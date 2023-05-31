// Fill out your copyright notice in the Description page of Project Settings.


#include "CharStatsBarWidget.h"


UCharStatsBarWidget::UCharStatsBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UCharStatsBarWidget::SetWidgetOwner(ICharStatsInterface* Owner)
{
	WidgetOwner = Owner;

	bDidInit = true;
}

void UCharStatsBarWidget::ClearWidgetOwner()
{
	bDidInit = false;

	WidgetOwner = nullptr;
}

float UCharStatsBarWidget::GetHealthRatio() const
{
	return bDidInit && WidgetOwner ? WidgetOwner->GetHealthRatio() : 0.0f;
}

float UCharStatsBarWidget::GetStaminaRatio() const
{
	return bDidInit && WidgetOwner ? WidgetOwner->GetStaminaRatio() : 0.0f;
}
