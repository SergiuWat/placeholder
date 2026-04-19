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
#include "CableComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "Enemies/EnemyBase.h"

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

	StartGrappleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("StartGrappleLocation"));
	StartGrappleLocation->SetupAttachment(GetRootComponent());

}


// Called when the game starts or when spawned
void AChameleonCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateHUDHealth();
	Tags.Add(FName("ChameleonCharacter"));
	OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	TransparentTrack.BindDynamic(this, &ThisClass::UpdateTransparentMaterial);

	ChameleonPlayerController = ChameleonPlayerController == nullptr ? Cast<AChameleonPlayerController>(Controller) : ChameleonPlayerController;
	if (ChameleonPlayerController)
	{
		ChameleonPlayerController->SetShowMouseCursor(true);
	}

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
	if (bIsChargingGrapple)
	{
		GrappleChargeTime += DeltaTime;

		float Alpha = FMath::Clamp(GrappleChargeTime / MaxChargeTime, 0.f, 1.f);
		//CurrentGrappleDistance = FMath::Lerp(0.f, MaxGrappleDistance, Alpha);
		CurrentGrappleDistance = Alpha * MaxGrappleDistance;
		UpdateGrapplePreview();
	}

	if (bIsHooked)
	{
		UpdateHookedTongue();

		UpdateSwing(DeltaTime);
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
		EnhancedInputComponent->BindAction(ClimbTongueAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::ClimbTongue);
		EnhancedInputComponent->BindAction(ClimbTongueAction, ETriggerEvent::Completed, this, &AChameleonCharacter::ClimbTongue);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::Look);
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::ClimbingLineTrace);
		EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Started, this, &AChameleonCharacter::StartGrappleCharge);
		EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Completed, this, &AChameleonCharacter::ReleaseGrappleCharge);
		EnhancedInputComponent->BindAction(TakeKeyAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::TakeKeyActionPressed);
		EnhancedInputComponent->BindAction(ShooTongueAction, ETriggerEvent::Triggered, this, &AChameleonCharacter::ShootTonguePressed);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void AChameleonCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (bIsHooked)
	{
		SwingInputValue = MovementVector.X;
		return;
	}

	SwingInputValue = 0.f;

	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if (bIsClimbing)
		{
			bool bFacingRight = GetActorForwardVector().X > 0;
			float ClimbInput = bFacingRight ? MovementVector.X : -MovementVector.X;
			AddMovementInput(GetActorUpVector(), ClimbInput);
		}
		else
		{
			AddMovementInput(ForwardDirection, MovementVector.X);
			//AddMovementInput(RightDirection, MovementVector.Y);
		}
	}
}

void AChameleonCharacter::ClimbTongue(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	if (!bIsHooked) return;

	GrappleRopeLength -= InputValue * RopeClimbSpeed * GetWorld()->GetDeltaSeconds();
	GrappleRopeLength = FMath::Clamp(GrappleRopeLength, MinRopeLength, MaxRopeLength);

	FVector PlayerLocation = GetActorLocation();
	PlayerLocation.Y = GrappleAnchorPoint.Y;

	FVector RopeVector = PlayerLocation - GrappleAnchorPoint;
	RopeVector.Y = 0.f;

	float CurrentDistance = RopeVector.Size();
	if (CurrentDistance <= KINDA_SMALL_NUMBER) return;

	FVector RopeDirection = RopeVector / CurrentDistance;

	FVector NewLocation = GrappleAnchorPoint + RopeDirection * GrappleRopeLength;
	NewLocation.Y = GetActorLocation().Y;

	SetActorLocation(NewLocation, true);
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


void AChameleonCharacter::InvisibleActionPressed()
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

void AChameleonCharacter::TakeKeyActionPressed(const FInputActionValue& Value)
{
	if (bHasKey) return;
	AActor* KeyActor = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Key"), FoundActors);

	if (FoundActors.Num() > 0)
	{
		KeyActor = FoundActors[0];
	}
	if (KeyActor && !bHasKey)
	{
		FRotator NewRotation = GetActorRotation();

		NewRotation.Yaw = (KeyActor->GetActorLocation().X <= 0.f) ? 0.f : 180.f;

		SetActorRotation(NewRotation);

		InitialTongueTransforms = CtrlTongueTransform;
		FVector InversedPosition = GetMesh()->GetComponentTransform().InverseTransformPosition(KeyActor->GetActorLocation());
		bTongueActive = true;
		CtrlTongueTransform = UKismetMathLibrary::MakeTransform(InversedPosition, FRotator::ZeroRotator, FVector(1.f));
		GetWorldTimerManager().SetTimer(KeyTimerHandle, [this, KeyActor]() {
			bHasKey = true;
			bTongueActive = false;
			CtrlTongueTransform = InitialTongueTransforms;
			KeyActor->Destroy();
			}, 0.2f, false);

	}
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
	//DrawDebugLine(GetWorld(), LowerStart, LowerEnd, bLowerHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 2.f);
	//DrawDebugLine(GetWorld(), UpperStart, UpperEnd, bUpperHit ? FColor::Blue : FColor::Yellow, false, 0.1f, 0, 2.f);


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


#pragma region Grapple

void AChameleonCharacter::StartGrappleCharge()
{
	if (bIsGrapplingActive) return;

	bIsChargingGrapple = true;
	GrappleChargeTime = 0.f;

	CalculateMouseDirection();
	UpdateFacingFromMouseDirection();

	TongueStartLocation = GetMesh()->GetBoneLocation(TEXT("tongue"));

	FVector AdjustedDirection = MouseDirection;
	AdjustedDirection.Y = 0.f;
	AdjustedDirection.Normalize();

	FVector Start = StartGrappleLocation->GetComponentLocation();
	FVector End = Start + MouseDirection * CurrentGrappleDistance;
	End.Y = Start.Y;

	TongueStartLocation = GetMesh()->GetBoneLocation(TEXT("tongue"));
	TongueTargetLocation = End;
	TongueTargetLocation.Y = TongueStartLocation.Y;

	UpdateTongueToWorldTarget(TongueTargetLocation);
}

void AChameleonCharacter::StartGrapple(float Distance)
{
	CalculateMouseDirection();
	UpdateFacingFromMouseDirection();
	FVector Start = StartGrappleLocation->GetComponentLocation();
	FVector End = Start + MouseDirection * Distance;


	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(25.f),
		QueryParams
	);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace HIT actor: %s"), *GetNameSafe(HitResult.GetActor()));

		if (HitResult.GetActor() && HitResult.GetActor()->ActorHasTag("GrapplePoint"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor has GrapplePoint tag"));

			bIsHooked = true;
			bTongueActive = true;
			bIsGrapplingActive = true;
			HookedLocation = HitResult.ImpactPoint;

			GrappleAnchorPoint = HookedLocation;
			GrappleAnchorPoint.Y = GetActorLocation().Y;

			float InitialDistanceToHook = FVector::Distance(GetActorLocation(), GrappleAnchorPoint);
			GrappleRopeLength = InitialDistanceToHook * 0.5f;
			GrappleRopeLength = FMath::Max(GrappleRopeLength, 100.f);

			FVector LaunchDirection = HookedLocation - GetActorLocation();
			LaunchDirection.Y = 0.f;
			LaunchDirection.Normalize();

			FVector LaunchVelocity = LaunchDirection * LaunchStrength;
			LaunchVelocity.Z += 150.f; 

			LaunchCharacter(LaunchVelocity, true, true);

			HangLocation = HookedLocation;
			HangLocation.Y = GetActorLocation().Y;
			HangLocation.Z -= HangDistanceBelowHook;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor does NOT have GrapplePoint tag"));

			bIsHooked = false;
			bTongueActive = false;
			bIsGrapplingActive = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace MISSED"));
		bIsHooked = false;
		bTongueActive = false;
		bIsGrapplingActive = false;
	}
}

void AChameleonCharacter::UpdateSwing(float DeltaTime)
{
	if (!bIsHooked) return;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp == nullptr) return;

	MoveComp->SetMovementMode(MOVE_Falling);

	FVector PlayerLocation = GetActorLocation();
	PlayerLocation.Y = GrappleAnchorPoint.Y;

	FVector RopeVector = PlayerLocation - GrappleAnchorPoint;
	RopeVector.Y = 0.f;

	float CurrentDistance = RopeVector.Size();
	if (CurrentDistance <= KINDA_SMALL_NUMBER) return;

	FVector RopeDirection = RopeVector / CurrentDistance;

	FVector Velocity = MoveComp->Velocity;
	Velocity.Y = 0.f;

	if (CurrentDistance > GrappleRopeLength)
	{
		float RadialSpeed = FVector::DotProduct(Velocity, RopeDirection);
		if (RadialSpeed > 0.f)
		{
			Velocity -= RopeDirection * RadialSpeed;
		}
	}

	if (!FMath::IsNearlyZero(SwingInputValue))
	{
		FVector Tangent(-RopeDirection.Z, 0.f, RopeDirection.X);
		Tangent.Normalize();

		Velocity += Tangent * SwingInputValue * SwingForce * DeltaTime;
	}

	Velocity.Y = 0.f;
	MoveComp->Velocity = Velocity;
}
void AChameleonCharacter::UpdateGrapplePreview()
{
	FVector Start = StartGrappleLocation->GetComponentLocation();

	CalculateMouseDirection();
	UpdateFacingFromMouseDirection();

	FVector End = Start + MouseDirection * CurrentGrappleDistance;
	End.Y = Start.Y;

	TongueStartLocation = GetMesh()->GetBoneLocation(TEXT("tongue"));
	TongueTargetLocation = End;
	TongueTargetLocation.Y = TongueStartLocation.Y;

	UpdateTongueToWorldTarget(TongueTargetLocation);

	//DrawDebugSphere(GetWorld(), End, 25.f, 12, FColor::Green, false, 0.f);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.f, 0, 2.f);
}

void AChameleonCharacter::UpdateHookedTongue()
{
	//if (!bIsHooked) return;

	TongueStartLocation = GetMesh()->GetBoneLocation(TEXT("tongue"));
	TongueTargetLocation = HookedLocation;
	TongueTargetLocation.Y = TongueStartLocation.Y;

	FRotator NewRotation = GetActorRotation();

	if (GetActorLocation().X < HookedLocation.X)
	{
		NewRotation.Yaw = 0.f;
	}
	else
	{
		NewRotation.Yaw = 180.f;
	}

	SetActorRotation(NewRotation);


	UpdateTongueToWorldTarget(TongueTargetLocation);

	/*DrawDebugSphere(GetWorld(), HookedLocation, 20.f, 12, FColor::Red, false, 0.f);
	DrawDebugLine(GetWorld(), TongueStartLocation, HookedLocation, FColor::Red, false, 0.f, 0, 2.f);*/
}

void AChameleonCharacter::CalculateMouseDirection()
{
	FVector Start = StartGrappleLocation->GetComponentLocation();
	FVector MouseWorldLocation;
	FVector MouseWorldDirection;

	ChameleonPlayerController = ChameleonPlayerController == nullptr
		? Cast<AChameleonPlayerController>(Controller)
		: ChameleonPlayerController;

	if (ChameleonPlayerController == nullptr) return;

	if (!ChameleonPlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
		return;

	if (FMath::IsNearlyZero(MouseWorldDirection.Y))
	{
		return;
	}

	float PlaneY = GetActorLocation().Y;

	float T = (PlaneY - MouseWorldLocation.Y) / MouseWorldDirection.Y;

	FVector MouseWorldPoint = MouseWorldLocation + MouseWorldDirection * T;
	MouseWorldPoint.Y = Start.Y;

	MouseDirection = MouseWorldPoint - Start;
	MouseDirection.Y = 0.f;
	MouseDirection.Normalize();
}

void AChameleonCharacter::ReleaseGrappleCharge()
{
	if (bIsHooked)
	{
		OnGrappleFinished();
		return;
	}

	if (!bIsChargingGrapple) return;

	bIsChargingGrapple = false;

	StartGrapple(CurrentGrappleDistance);
}
void AChameleonCharacter::OnGrappleFinished()
{
	bIsGrapplingActive = false;
	bIsChargingGrapple = false;
	bTongueActive = false;
	bIsHooked = false;

	SwingInputValue = 0.f;
	GrappleAnchorPoint = FVector::ZeroVector;
	GrappleRopeLength = 0.f;

	GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	CtrlTongueTransform = FTransform::Identity;
}

void AChameleonCharacter::UpdateTongueToWorldTarget(const FVector& WorldTargetLocation)
{
	if (!GetMesh()) return;

	const FVector InversedPosition =
		GetMesh()->GetComponentTransform().InverseTransformPosition(WorldTargetLocation);

	bTongueActive = true;
	CtrlTongueTransform = UKismetMathLibrary::MakeTransform(
		InversedPosition,
		FRotator::ZeroRotator,
		FVector(1.f)
	);
}

void AChameleonCharacter::UpdateFacingFromMouseDirection()
{
	if (MouseDirection.IsNearlyZero()) return;

	FRotator NewRotation = GetActorRotation();

	NewRotation.Yaw = (MouseDirection.X >= 0.f) ? 0.f : 180.f;

	SetActorRotation(NewRotation);
}

void AChameleonCharacter::ShootTonguePressed()
{
	if (!bCanShootTongue) return;
	CalculateMouseDirection();
	UpdateFacingFromMouseDirection();
	bCanShootTongue = false;
	if (MouseDirection.IsNearlyZero()) return;

	FVector Start = StartGrappleLocation
		? StartGrappleLocation->GetComponentLocation()
		: GetMesh()->GetBoneLocation(TEXT("tongue"));

	Start.Y = GetActorLocation().Y;

	FVector Direction = MouseDirection;
	Direction.Y = 0.f;
	Direction.Normalize();

	FVector End = Start + Direction * TongueShootDistance;
	End.Y = Start.Y;

	bTongueActive = true;
	TongueStartLocation = GetMesh()->GetBoneLocation(TEXT("tongue"));
	TongueTargetLocation = End;
	TongueTargetLocation.Y = TongueStartLocation.Y;

	UpdateTongueToWorldTarget(TongueTargetLocation);


	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	if (bHit && HitResult.GetActor())
	{
		TongueTargetLocation = HitResult.ImpactPoint;
		TongueTargetLocation.Y = TongueStartLocation.Y;
		UpdateTongueToWorldTarget(TongueTargetLocation);

		AEnemyBase* Enemy = Cast<AEnemyBase>(HitResult.GetActor());
		if (Enemy)
		{
			UGameplayStatics::ApplyDamage(
				Enemy,
				TongueDamage,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
		}

		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 14.f, 12, FColor::Red, false, 1.0f);
	}

	GetWorldTimerManager().ClearTimer(TongueShootTimerHandle);
	GetWorldTimerManager().SetTimer(
		TongueShootTimerHandle,
		this,
		&AChameleonCharacter::ResetTongueAttack,
		TongueShootCooldown,
		false
	);
}

void AChameleonCharacter::ResetTongueAttack()
{
	bTongueActive = false;
	bCanShootTongue = true;
	CtrlTongueTransform = FTransform::Identity;
}

#pragma endregion 
