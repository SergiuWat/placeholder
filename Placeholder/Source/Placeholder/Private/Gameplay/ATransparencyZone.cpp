// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ATransparencyZone.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/ChameleonCharacter.h"


AATransparencyZone::AATransparencyZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AATransparencyZone::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	
}

void AATransparencyZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AChameleonCharacter* Player = Cast<AChameleonCharacter>(OtherActor);
	if (Player)
	{
		Player->SetCanPlayerBecomeTransparent(true);
	}
}

void AATransparencyZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AChameleonCharacter* Player = Cast<AChameleonCharacter>(OtherActor);
	if (Player)
	{
		Player->SetCanPlayerBecomeTransparent(false);
		Player->StopTransparent();
	}
}

// Called every frame
void AATransparencyZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

