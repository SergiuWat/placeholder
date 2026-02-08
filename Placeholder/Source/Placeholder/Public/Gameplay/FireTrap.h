// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireTrap.generated.h"


class UBoxComponent;
UENUM(BlueprintType)
enum class ERotationMode :uint8
{
	Full360,
	LeftRight
};

UCLASS()
class PLACEHOLDER_API AFireTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireTrap();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Pivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RotatingMesh;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	ERotationMode RotationMode = ERotationMode::Full360;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 120.f;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float MaxAngle = 90.f;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 15.f;

	UPROPERTY(EditAnywhere, Category = "Knockback")
	float KnockBackStrength = 800.f;

	UPROPERTY(EditAnywhere, Category = "Knockback")
	float KnockbackUpForce = 300.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	float CurrentAngle = 0.f;
	int Direction = 1;
	void UpdateRotation(float DeltaTime);
};
