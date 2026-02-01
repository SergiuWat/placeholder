// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ChameleonCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class AChameleonPlayerController;
class UAnimMontage;

UCLASS()
class PLACEHOLDER_API AChameleonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AChameleonCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* FollowCamera;


	/*
	* Input actions
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	/** Hud Functions */

	void UpdateHUDHealth();


	/** Animation Montages */
	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathMontage;

	void PlayDeathMontage();

	UFUNCTION(BlueprintCallable)
	void SetPlayerDeathFinished();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCause);

private:

	UPROPERTY()
	AChameleonPlayerController* ChameleonPlayerController;

	/** Player health and Shield*/
	UPROPERTY(EditAnywhere, Category = "Player Starts")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Player Starts")
	float Health = 100.f;

	bool bIsPlayerDead = false;
	bool bDeathFinished = false;

public: // This public should be used for getters and setters or inlinefunctions

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE bool IsPlayerDeath() { return bIsPlayerDead; }
	FORCEINLINE bool IsPlayerDeathFinished() { return bDeathFinished; }
};
