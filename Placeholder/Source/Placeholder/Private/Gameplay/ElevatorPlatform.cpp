// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ElevatorPlatform.h"
#include "Components/BoxComponent.h"
#include "Components/InterpToMovementComponent.h"

// Sets default values
AElevatorPlatform::AElevatorPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;


	BasePlatform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BasePlatform"));
	BasePlatform->SetupAttachment(GetRootComponent());


	MovementComponent = CreateDefaultSubobject<UInterpToMovementComponent>(TEXT("Movement Component"));


	MovementComponent->Duration = 5.f;
	MovementComponent->bSweep = false;
	MovementComponent->BehaviourType = EInterpToBehaviourType::PingPong;
	MovementComponent->bAutoActivate = false;

}

// Called when the game starts or when spawned
void AElevatorPlatform::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent->Activate();
	for (int i = 0; i < ThePath.Num(); i++)
	{
		MovementComponent->ControlPoints.Add(FInterpControlPoint(ThePath[i], true));
	}
	MovementComponent->FinaliseControlPoints();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	
}

// Called every frame
void AElevatorPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AElevatorPlatform::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("ChameleonCharacter"))
	{
		//MovementComponent->Activate();
		UE_LOG(LogTemp, Warning, TEXT("Character detected"));
		//MovePlatformUp();
	}
}

void AElevatorPlatform::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap"));
	//MovePlateformDown();
}


