// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/UnarmedEnemy.h"
#include "Components/BoxComponent.h"
#include "Characters/ChameleonCharacter.h"
#include "Placeholder.h"
#include "Kismet/GameplayStatics.h"
AUnarmedEnemy::AUnarmedEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	PunchBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	PunchBoxCollision->SetupAttachment(GetMesh(), TEXT("hand_r"));

	PunchBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PunchBoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PunchBoxCollision->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Overlap);
	PunchBoxCollision->SetGenerateOverlapEvents(true);
}

void AUnarmedEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (PunchBoxCollision)
	{

		PunchBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnPunchOverlap);
	}
}

void AUnarmedEnemy::OnPunchOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->ActorHasTag(FName("ChameleonCharacter")))
	{		
		UGameplayStatics::ApplyDamage(OtherActor, PunchDamage, GetController(), this, UDamageType::StaticClass());
	}
}

void AUnarmedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUnarmedEnemy::EnablePunchBoxCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("Enabled collision"));
	PunchBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AUnarmedEnemy::DisablePunchBoxCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("Disable collision"));
	PunchBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
