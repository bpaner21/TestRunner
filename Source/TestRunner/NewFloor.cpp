// Fill out your copyright notice in the Description page of Project Settings.

#include "NewFloor.h"
#include "TestRunner.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h" 
//#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h""
#include "Runtime/Engine/Classes/Engine/CollisionProfile.h"

// Sets default values
ANewFloor::ANewFloor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RockMesh(TEXT("/Game/ThirdPerson/Meshes/PlatformStaticMesh.PlatformStaticMesh"));

	// Create the mesh component
	FloorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));
	RootComponent = FloorMeshComponent;
	FloorMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	FloorMeshComponent->SetStaticMesh(RockMesh.Object);

	FloorMeshComponent->SetWorldScale3D(FVector(2.0F, 2.0F, 0.2F));

	FloorBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
	FloorBoxComponent->SetCollisionProfileName("Trigger");
	RootComponent = FloorBoxComponent;//*/

	FloorBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ANewFloor::OnOverlap);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANewFloor::BeginPlay()
{
	Super::BeginPlay();
	
	//SetLifeSpan(10.0f);
}

// Called every frame
void ANewFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANewFloor::OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		Destroy();
	}
}