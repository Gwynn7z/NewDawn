#include "MedkitInventoryItem.h"
#include "../Archer.h"
#include "Inventory.h"
#include "UObject/ConstructorHelpers.h"

UMedkitInventoryItem::UMedkitInventoryItem()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> MedkitAsset(TEXT("Texture2D'/Game/HUD/Textures/Medkit.Medkit'"));
	if (MedkitAsset.Object != NULL) SetItemTexture(MedkitAsset.Object);
	
	SetUsableType(EUsableTypes::Medkit);
	Heal = 25.f;
}

void UMedkitInventoryItem::Use()
{
	if (GetItemOwner())
	{
		bool Succeded = GetItemOwner()->Heal(Heal);
		if (!Succeded) return;
	}
	Super::Use();
}