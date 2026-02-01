// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/EnemyAnimInstance.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	EnemyCharacter = Cast<AEnemyBase>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (EnemyCharacter == nullptr)
	{
		EnemyCharacter = Cast<AEnemyBase>(TryGetPawnOwner());
	}
	if (EnemyCharacter == nullptr) return;

	FVector Velocity = EnemyCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = EnemyCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = EnemyCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;

}
