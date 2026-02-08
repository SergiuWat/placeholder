// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FireTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Placeholder.h"
#include "Characters/ChameleonCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFireTrap::AFireTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(Root);

	RotatingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotatingMesh"));
	RotatingMesh->SetupAttachment(Pivot);

	RotatingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RotatingMesh->SetGenerateOverlapEvents(true);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	CollisionBox->SetupAttachment(Pivot);

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Overlap);
	CollisionBox->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AFireTrap::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	
}


// Called every frame
void AFireTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateRotation(DeltaTime);

}

void AFireTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap detected"));
	AChameleonCharacter* Player = Cast<AChameleonCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Cast"));
		UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, UDamageType::StaticClass());
		FVector KnockDirection = Player->GetActorLocation() - GetActorLocation();
		KnockDirection.Z = 0.f;
		KnockDirection.Normalize();

		KnockBackStrength = 800.f;
		KnockbackUpForce = 300.f;
		FVector LaunchVelocity = KnockDirection * KnockBackStrength;
		LaunchVelocity.Z = KnockbackUpForce;

		Player->LaunchCharacter(LaunchVelocity, true, true);
	}
}


void AFireTrap::UpdateRotation(float DeltaTime)
{
	if (RotationMode == ERotationMode::Full360)
	{
		CurrentAngle += RotationSpeed * DeltaTime;
	}
	else
	{
		CurrentAngle += Direction * RotationSpeed * DeltaTime;
		if (CurrentAngle >= MaxAngle)
		{
			CurrentAngle = MaxAngle;
			Direction =- 1;
		}
		else if (CurrentAngle <= -MaxAngle)
		{
			CurrentAngle = -MaxAngle;
			Direction = 1;
		}
	}
	Pivot->SetRelativeRotation(FRotator(CurrentAngle, 0.f, 0.f));
}


