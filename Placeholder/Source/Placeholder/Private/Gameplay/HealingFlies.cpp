// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/HealingFlies.h"

// Sets default values
AHealingFlies::AHealingFlies()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FliesMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FilesMesh"));
	SetRootComponent(FliesMesh);
	Tags.Add("HealingFlies");
}

// Called when the game starts or when spawned
void AHealingFlies::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHealingFlies::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

