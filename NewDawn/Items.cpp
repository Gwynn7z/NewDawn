#include "Items.h"
#include "Components/StaticMeshComponent.h"
#include "Items/UsableInventoryItem.h"

AItems::AItems()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OverlapComponent"));
	SetRootComponent(Mesh);
}

void AItems::RemoveFromStack(int32 StackToRemove)
{
	ItemValues.Value -= StackToRemove;
}

TTuple<EUsableTypes, int32> AItems::GetUsableItem()
{
	return ItemValues;
}
