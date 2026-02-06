// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ChameleonCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "ChameleonControllers/ChameleonPlayerController.h"

// Sets default values
AChameleonCharacter::AChameleonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	TransparentTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TransparentTimelineComponent"));

}

// Called when the game starts or when spawned
void AChameleonCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateHUDHealth();
	Tags.Add(FName("ChameleonCharacter"));
	OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	TransparentTrack.BindDynamic(this, &ThisClass::UpdateTransparentMaterial);
	if (TransparentCurve)
	{
		TransparentTimeline->AddInterpFloat(TransparentCurve, TransparentTrack);
	}
}


// Called every frame
void AChameleonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AChameleonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::Look);
		EnhancedInputComponent->BindAction(InvisibleAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::InvisibleActionPressed);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void AChameleonCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.X);
		// This might not be needed
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void AChameleonCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * -1);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AChameleonCharacter::InvisibleActionPressed(const FInputActionValue& Value)
{
	if (!bCanPlayerBecomeTransparent) return;

	if (TransparentMaterialInstance_1 && TransparentMaterialInstance_2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dynamic material created"));
		DynamicTransparentMaterialInstance_1 = UMaterialInstanceDynamic::Create(TransparentMaterialInstance_1, this);
		DynamicTransparentMaterialInstance_2 = UMaterialInstanceDynamic::Create(TransparentMaterialInstance_2, this);

		GetMesh()->SetMaterial(0, DynamicTransparentMaterialInstance_1);
		GetMesh()->SetMaterial(1, DynamicTransparentMaterialInstance_2);
		DynamicTransparentMaterialInstance_1->SetScalarParameterValue(TEXT("Opacity"), 1.f);
		DynamicTransparentMaterialInstance_2->SetScalarParameterValue(TEXT("Opacity"), 1.f);
	}
	bIsCharacterTransparent ? StopTransparent() : StartTransparent();

}

void AChameleonCharacter::UpdateHUDHealth()
{
	ChameleonPlayerController = ChameleonPlayerController == nullptr ? Cast<AChameleonPlayerController>(Controller) : ChameleonPlayerController;
	if (ChameleonPlayerController)
	{
		ChameleonPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}


void AChameleonCharacter::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCause)
{
	if (bIsPlayerDead) return;
	UE_LOG(LogTemp, Warning, TEXT("Player has been hit"));
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	if (Health == 0.f)
	{
		bIsPlayerDead = true;
		DisableInput(Cast<APlayerController>(Controller));
		GetCharacterMovement()->DisableMovement();

		PlayDeathMontage();
	}
}

void AChameleonCharacter::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage, 1.0f);
	}
}

void AChameleonCharacter::SetPlayerDeathFinished()
{
	bDeathFinished = true;
}

void AChameleonCharacter::UpdateTransparentMaterial(float TransparentValue)
{
	if (DynamicTransparentMaterialInstance_1 && DynamicTransparentMaterialInstance_2)
	{
		DynamicTransparentMaterialInstance_1->SetScalarParameterValue(TEXT("Opacity"), TransparentValue);
		DynamicTransparentMaterialInstance_2->SetScalarParameterValue(TEXT("Opacity"), TransparentValue);
	}
}

void AChameleonCharacter::StartTransparent()
{

	if (TransparentCurve && TransparentTimeline)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		bIsCharacterTransparent = true;
		OnTransparencyChanged.Broadcast(true);
		TransparentTimeline->Play();
	}
}

void AChameleonCharacter::StopTransparent()
{
	if (TransparentCurve && TransparentTimeline)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		bIsCharacterTransparent = false;
		OnTransparencyChanged.Broadcast(false);
		TransparentTimeline->Reverse();
	}
}


