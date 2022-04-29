#include "Bag.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"

ABag::ABag()
{
	PrimaryActorTick.bCanEverTick = false;
	BagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BagMesh"));
	SetRootComponent(BagMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("StaticMesh'/Game/Bag.Bag'"));
	BagMesh->SetStaticMesh(StaticMeshAsset.Object);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	TriggerBox->SetRelativeScale3D(FVector(9.f, 9.f, 5.f));

	Inventory = FUnownedInventory();
}

FUnownedInventory* ABag::GetInventory()
{
	return &Inventory;
}

void ABag::AddToInventory(FUnownedInventory NewInventory)
{
	Inventory.UpdateNewInventory(NewInventory, EModifierType::Increase);
}
