// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/TimelineComponent.h"
#include "ChameleonCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class AChameleonPlayerController;
class UAnimMontage;
class UCableComponent;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnTransparencyChanged, bool);

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

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InvisibleAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ClimbAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* GrappleAction;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void InvisibleActionPressed(const FInputActionValue& Value);

	/** Hud Functions */

	void UpdateHUDHealth();


	/** Animation Montages */
	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathMontage;

	void PlayDeathMontage();

	UFUNCTION(BlueprintCallable)
	void SetPlayerDeathFinished();

	/**
	Transparent effect
	*/

	FOnTransparencyChanged OnTransparencyChanged;

	bool bIsCharacterTransparent = false;

	bool bCanPlayerBecomeTransparent = false;

	UPROPERTY(VisibleAnywhere, Category = "Transparent")
	UTimelineComponent* TransparentTimeline;

	FOnTimelineFloat TransparentTrack;

	UPROPERTY(EditAnywhere, Category = "Transparent")
	UCurveFloat* TransparentCurve;

	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = "Transparent")
	UMaterialInstanceDynamic* DynamicTransparentMaterialInstance_1;

	UPROPERTY(VisibleAnywhere, Category = "Transparent")
	UMaterialInstanceDynamic* DynamicTransparentMaterialInstance_2;

	// Material instance set on the blueprint used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = "Transparent")
	UMaterialInstance* TransparentMaterialInstance_1;

	UPROPERTY(EditAnywhere, Category = "Transparent")
	UMaterialInstance* TransparentMaterialInstance_2;

	UFUNCTION()
	void UpdateTransparentMaterial(float TransparentValue);

	void StartTransparent();
	void StopTransparent();

	/*
	* Climbing 
	*/

	void ClimbingLineTrace();
	void CheckClimbSurface();
	void StartMantle(const FHitResult& WallHit);
	void StopClimbing();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* StartTrace;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* LedgeTrace;
	bool bIsClimbing = false;
	float LaunchForwardDistance = 50.f;
	float LaunchHeightDistance = 185.f;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ClimbMontage;

	void PlayClimbMontage();

	/*
	* Grapple
	*/
	void StartGrapple();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* StartGrappleLocation;

	UPROPERTY(VisibleAnywhere, Category = "Cable")
	UCableComponent* CableComponent;

	bool bIsGrapplingActive = false;

	UFUNCTION()
	void OnGrappleFinished();

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
	FORCEINLINE void SetCanPlayerBecomeTransparent(bool CanBeTransparent) { bCanPlayerBecomeTransparent = CanBeTransparent; }
	FORCEINLINE bool IsPlayerDeath() { return bIsPlayerDead; }
	FORCEINLINE bool IsPlayerDeathFinished() { return bDeathFinished; }
	FORCEINLINE bool IsCharacterTransparent() { return bIsCharacterTransparent; }
	FORCEINLINE bool IsCharacterClimbing() { return bIsClimbing; }

};
