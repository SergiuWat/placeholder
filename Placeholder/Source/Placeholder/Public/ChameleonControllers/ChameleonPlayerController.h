// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChameleonPlayerController.generated.h"

class AChameleonHUD;
/**
 * 
 */
UCLASS()
class PLACEHOLDER_API AChameleonPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	void SetHUDHealth(float Health, float MaxHealth);

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	AChameleonHUD* ChameleonHUD;
};
