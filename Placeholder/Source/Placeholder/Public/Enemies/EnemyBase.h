// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyTypes/EnemyTypes.h"
#include "EnemyBase.generated.h"

class UPawnSensingComponent;
class AAIController;
class AAIController;
class AnimMontage;
UCLASS()
class PLACEHOLDER_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AttackMontageFinished();

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	/*
	* Timers
	*/

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();
	/*
	* Navigation
	*/

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category ="AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	// The distance between the enemy and his target
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius = 15.f;

	UPROPERTY(EditAnywhere)
	float PatrolRadius = 200.f;

	double patrolRange;

	void CheckPatrolTarget();

	/*
	*  Play Animation Montages
	*/
	void PlayAttackMontage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSensed(APawn* PawnSensed);

	/*
	*  Animation Montages
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

private:	

	UPROPERTY(EditAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	/*
	*  Combat
	*/
	UPROPERTY();
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	float CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.f;

	void CheckCombatTarget();

	
};
