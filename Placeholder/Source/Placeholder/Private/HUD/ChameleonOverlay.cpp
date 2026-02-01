// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChameleonOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UChameleonOverlay::SetProgressBarHealth(float Health, float MaxHealth)
{
	const float HealthPercent = Health / MaxHealth;
	HealthProgressBar->SetPercent(HealthPercent);
	FString TextHealth = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
	HealthText->SetText(FText::FromString(TextHealth));
}
