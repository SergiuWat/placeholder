// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "UnarmedEnemy.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class PLACEHOLDER_API AUnarmedEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	AUnarmedEnemy();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void EnablePunchBoxCollision();

	UFUNCTION(BlueprintCallable)
	void DisablePunchBoxCollision();

	UPROPERTY(EditAnywhere)
	float PunchDamage = 20.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPunchOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* PunchBoxCollision;
	
};
