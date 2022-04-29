#include "Chest.h"
#include "Components/StaticMeshComponent.h"

AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;

	TrialMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrialMesh"));
	SetRootComponent(TrialMesh);
}

void AChest::BeginPlay()
{
	Super::BeginPlay();

	Inventory = FUnownedInventory();
	Inventory.UpdateResources(Resources, EModifierType::Increase);
	Inventory.UpdateAmmunitions(Ammunitions, EModifierType::Increase);
	Inventory.UpdateUsables(Usables, EModifierType::Increase);
}

FUnownedInventory* AChest::GetInventory() {
	return &Inventory;
};