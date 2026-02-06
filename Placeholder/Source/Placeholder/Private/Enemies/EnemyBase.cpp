// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "Characters/ChameleonCharacter.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComponent->SightRadius = 400.f;
	PawnSensingComponent->SetPeripheralVisionAngle(45.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CachedPlayer = Cast<AChameleonCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if(CachedPlayer)
	{
		CachedPlayer->OnTransparencyChanged.AddUObject(this, &ThisClass::OnPlayerTransparencyChanged);
	}
	
	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &ThisClass::PawnSensed);
	}
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);	

}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}

}

void AEnemyBase::AttackMontageFinished()
{
	EnemyState = EEnemyState::EES_Chasing;
}

void AEnemyBase::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ThisClass::PatrolTimerFinished, 5.f);
	}
}

void AEnemyBase::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		// Outside combat radius, lose interest
		LoseInterest();

	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		// Outside attack range, chase character
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
		MoveToTarget(CombatTarget);

	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState !=EEnemyState::EES_Attacking)
	{
		// Inside attack range, attack character
		EnemyState = EEnemyState::EES_Attacking;
		PlayAttackMontage();
	}
}


bool AEnemyBase::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemyBase::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);	
}

AActor* AEnemyBase::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{

		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];

	}
	return nullptr;
}


void AEnemyBase::PawnSensed(APawn* PawnSensed)
{
	if (!CachedPlayer) return;
	if (EnemyState == EEnemyState::EES_Chasing) return;
	if (PawnSensed->ActorHasTag(FName("ChameleonCharacter")))
	{
		if (CachedPlayer->IsCharacterTransparent()) return;

		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
		}

		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
		CombatTarget = PawnSensed;
		MoveToTarget(CombatTarget);
	}

}

void AEnemyBase::OnPlayerTransparencyChanged(bool bIsTransparent)
{
	if (bIsTransparent)
	{
		LoseInterest();
	}
}

void AEnemyBase::LoseInterest()
{
	CombatTarget = nullptr;
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	MoveToTarget(ChoosePatrolTarget());
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &ThisClass::PatrolTimerFinished, 5.f);
}

void AEnemyBase::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemyBase::PlayAttackMontage()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.0f);
	}
}




