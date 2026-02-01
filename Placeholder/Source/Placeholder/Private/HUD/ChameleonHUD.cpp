// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChameleonHUD.h"
#include "GameFramework/PlayerController.h"
#include "HUD/ChameleonOverlay.h"


void AChameleonHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AChameleonHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AChameleonHUD::AddChameleonOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && ChameleonOverlayClass)
	{
		ChameleonOverlay = CreateWidget<UChameleonOverlay>(PlayerController, ChameleonOverlayClass);
		ChameleonOverlay->AddToViewport();
	}
}
