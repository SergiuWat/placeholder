// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ChameleonHUD.generated.h"

class UUserWidget;
class UChameleonOverlay;
/**
 * 
 */
UCLASS()
class PLACEHOLDER_API AChameleonHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ChameleonOverlayClass;

	UPROPERTY()
	UChameleonOverlay* ChameleonOverlay;

	void AddChameleonOverlay();

protected:
	virtual void BeginPlay() override;
};
