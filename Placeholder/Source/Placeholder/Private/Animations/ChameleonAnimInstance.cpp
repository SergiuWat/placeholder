// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/ChameleonAnimInstance.h"
#include "Characters/ChameleonCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UChameleonAnimInstance::NativeInitializeAnimation()
{
	PlayerCharacter = Cast<AChameleonCharacter>(TryGetPawnOwner());
}

void UChameleonAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (PlayerCharacter == nullptr)
	{
		PlayerCharacter = Cast<AChameleonCharacter>(TryGetPawnOwner());
	}

	if (PlayerCharacter == nullptr) return;

	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
}
