// Fill out your copyright notice in the Description page of Project Settings.


#include "ChameleonControllers/ChameleonPlayerController.h"
#include "HUD/ChameleonHUD.h"
#include "HUD/ChameleonOverlay.h"

void AChameleonPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ChameleonHUD = Cast<AChameleonHUD>(GetHUD());
	if (ChameleonHUD)
	{
		ChameleonHUD->AddChameleonOverlay();
	}
}


void AChameleonPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChameleonPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	ChameleonHUD = ChameleonHUD == nullptr ? Cast<AChameleonHUD>(GetHUD()) : ChameleonHUD;

	bool bHUDValid = ChameleonHUD &&
		ChameleonHUD->ChameleonOverlay &&
		ChameleonHUD->ChameleonOverlay->HealthProgressBar &&
		ChameleonHUD->ChameleonOverlay->HealthText;

	if (bHUDValid)
	{
		ChameleonHUD->ChameleonOverlay->SetProgressBarHealth(Health, MaxHealth);
	}
}

