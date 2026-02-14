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
#include "Kismet/KismetSystemLibrary.h"

#include "DrawDebugHelpers.h"

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

	StartTrace = CreateDefaultSubobject<USceneComponent>(TEXT("StartTrace"));
	StartTrace->SetupAttachment(GetRootComponent());

	LedgeTrace = CreateDefaultSubobject<USceneComponent>(TEXT("LedgeTrace"));
	LedgeTrace->SetupAttachment(GetRootComponent());

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
	if (bIsClimbing)
	{
		CheckClimbSurface();
	}
}

#pragma region Input Actions

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
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::ClimbingLineTrace);
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
		if (bIsClimbing)
		{
			bool bFacingRight = GetActorForwardVector().X > 0;
			float ClimbInput = bFacingRight ? MovementVector.X : -MovementVector.X;
			AddMovementInput(GetActorUpVector(), ClimbInput);
		}
		else {
			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.X);
			// This might not be needed
			AddMovementInput(RightDirection, MovementVector.Y);
		}		
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

#pragma endregion

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


#pragma region Transparent


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

#pragma endregion

#pragma region Climbing

void AChameleonCharacter::ClimbingLineTrace()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;
		StopClimbing();
		return;
	}
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FVector TraceLength = GetActorForwardVector() * 100.f;
	FVector End = TraceLength + GetActorLocation();
	GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace->GetComponentLocation() , End, ECC_Visibility, QueryParams);

	if (HitResult.bBlockingHit)
	{
		bIsClimbing = true;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		GetCharacterMovement()->MaxFlySpeed = 200.f;
		GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void AChameleonCharacter::CheckClimbSurface()
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FVector LowerStart = StartTrace->GetComponentLocation();
	FVector LowerEnd = LowerStart + GetActorForwardVector() * 80.f;

	FHitResult LowerHit;
	bool bLowerHit = GetWorld()->LineTraceSingleByChannel(
		LowerHit,
		LowerStart,
		LowerEnd,
		ECC_Visibility,
		QueryParams
	);

	FVector UpperStart = LedgeTrace->GetComponentLocation();
	FVector UpperEnd = UpperStart + GetActorForwardVector() * 80.f;

	FHitResult UpperHit;
	bool bUpperHit = GetWorld()->LineTraceSingleByChannel(
		UpperHit,
		UpperStart,
		UpperEnd,
		ECC_Visibility,
		QueryParams
	);

	// debug
	DrawDebugLine(GetWorld(), LowerStart, LowerEnd, bLowerHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 2.f);
	DrawDebugLine(GetWorld(), UpperStart, UpperEnd, bUpperHit ? FColor::Blue : FColor::Yellow, false, 0.1f, 0, 2.f);


	if (bLowerHit && !bUpperHit)
	{
		StartMantle(LowerHit);
	}
	else if (!bLowerHit)
	{
		StopClimbing();
	}
}

void AChameleonCharacter::StartMantle(const FHitResult& WallHit)
{

	bIsClimbing = false;
	StopClimbing();

	FVector TargetLocation =
		WallHit.ImpactPoint +
		GetActorForwardVector() * 40.f +
		FVector(0, 0, LaunchHeightDistance);

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "OnMantleFinished";
	LatentInfo.UUID = 1;

	UKismetSystemLibrary::MoveComponentTo(
		GetCapsuleComponent(),
		TargetLocation,
		GetActorRotation(),
		true,
		true,
		1.f,
		false,
		EMoveComponentAction::Move,
		LatentInfo
	);
	PlayClimbMontage();
	StopClimbing();

}

void AChameleonCharacter::StopClimbing()
{
	bIsClimbing = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
}


void AChameleonCharacter::PlayClimbMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ClimbMontage)
	{
		AnimInstance->Montage_Play(ClimbMontage, 1.0f);
	}
}

#pragma endregion 
