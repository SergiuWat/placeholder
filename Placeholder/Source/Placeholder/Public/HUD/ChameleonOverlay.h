// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChameleonOverlay.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;
/**
 * 
 */
UCLASS()
class PLACEHOLDER_API UChameleonOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UImage* ChameleonImage;

	void SetProgressBarHealth(float Health, float MaxHealth);
};
