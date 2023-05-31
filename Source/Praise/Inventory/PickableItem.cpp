// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APickableItem::APickableItem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoxCol = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	SetRootComponent(BoxCol);
	BoxCol->SetBoxExtent(FVector(100, 100, 100));
	BoxCol->SetCollisionProfileName(FName("Pawn"));
	BoxCol->SetEnableGravity(true);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	MeshComp->AttachTo(RootComponent);
	MeshComp->SetCollisionProfileName(FName("NoCollision"));

	

}

// Called when the game starts or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

