#include "ArrowInventoryItem.h"
#include "UObject/ConstructorHelpers.h"
#include "ResourcesInventoryItem.h"
#include "../WeaponWidget.h"
#include "../Archer.h"
#include "Inventory.h"

UArrowInventoryItem::UArrowInventoryItem()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> NormalAsset(TEXT("Texture2D'/Game/HUD/Textures/NormalArrow.NormalArrow'"));
	if (NormalAsset.Object != NULL) NormalArrowTexture = NormalAsset.Object;
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> TearAsset(TEXT("Texture2D'/Game/HUD/Textures/TearArrow.TearArrow'"));
	if (TearAsset.Object != NULL) TearArrowTexture = TearAsset.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> FireAsset(TEXT("Texture2D'/Game/HUD/Textures/FireArrow.FireArrow'"));
	if (FireAsset.Object != NULL) FireArrowTexture = FireAsset.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> IceAsset(TEXT("Texture2D'/Game/HUD/Textures/IceArrow.IceArrow'"));
	if (IceAsset.Object != NULL) IceArrowTexture = IceAsset.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> ElectricAsset(TEXT("Texture2D'/Game/HUD/Textures/ElectricArrow.ElectricArrow'"));
	if (ElectricAsset.Object != NULL) ElectricArrowTexture = ElectricAsset.Object;

	SetMaxOneSlot(true);
	SetShowType(EShowType::All);
	EquipEnabled = true;
	EquipText = FText::FromString("Craft");
}

void UArrowInventoryItem::Equip()
{
	if (ItemLocation != nullptr && ItemOwner != nullptr)
	{
		int32 ArrowNumber = ItemOwner->ArrowOutOfQuiver() ? GetStackNumber() + 1 : GetStackNumber();
		if (ArrowNumber >= GetSlotCapacity()) {
			return;
		}
		else {
			if (ItemLocation->CheckRemoveResources(CraftingNeeds)) {
				ItemLocation->AddArrows(GetArrowType(), 5);
				if (WeaponWidget != nullptr) WeaponWidget->UpdateStack();
			}
			else ItemOwner->AddLog("Resources Insufficient");
		}
	}
	Super::Equip();
}

void UArrowInventoryItem::SetArrowType(EDamageTypes NewType)
{
	DamageType = NewType;
	switch (DamageType)
	{
	case EDamageTypes::Physical:
		SetItemName("Normal Arrow");
		SetItemDescription("Basic arrows, useful to destroy machines");
		SetSlotCapacity(25);
		SetItemTexture(NormalArrowTexture);
		break;
	case EDamageTypes::Tear:
		SetItemName("Tear Arrow");
		SetItemDescription("Arrow capable of breaking and taking out specific parts of machines");
		SetSlotCapacity(15);
		SetItemTexture(TearArrowTexture);
		break;
	case EDamageTypes::Fire:
		SetItemName("Fire Arrow");
		SetItemDescription("Put enemies on fire with this");
		SetSlotCapacity(10);
		SetItemTexture(FireArrowTexture);
		break;
	case EDamageTypes::Ice:
		SetItemName("Ice Arrow");
		SetItemDescription("Lower the temperature of interal machine's circuits. Electrical components won't endure cold");
		SetSlotCapacity(10);
		SetItemTexture(IceArrowTexture);
		break;
	case EDamageTypes::Electric:
		SetItemName("Electric Arrow");
		SetItemDescription("Short-Circuit machine's circuits and knock them out");
		SetSlotCapacity(10);
		SetItemTexture(ElectricArrowTexture);
		break;
	default:
		break;
	}

	switch (DamageType) 
	{
	case EDamageTypes::Physical:
		CraftingNeeds.Add(EResourceTypes::Wood, 2);
		CraftingNeeds.Add(EResourceTypes::Wire, 1);
		CraftingNeeds.Add(EResourceTypes::CopperFragment, 5);
		CraftingNeeds.Add(EResourceTypes::ElectronicComponent, 0);
		CraftingNeeds.Add(EResourceTypes::MetalPlate, 0);
		break;
	case EDamageTypes::Tear:
		CraftingNeeds.Add(EResourceTypes::Wood, 4);
		CraftingNeeds.Add(EResourceTypes::Wire, 1);
		CraftingNeeds.Add(EResourceTypes::CopperFragment, 5);
		CraftingNeeds.Add(EResourceTypes::ElectronicComponent, 0);
		CraftingNeeds.Add(EResourceTypes::MetalPlate, 1);
		break;
	default:
		CraftingNeeds.Add(EResourceTypes::Wood, 2);
		CraftingNeeds.Add(EResourceTypes::Wire, 2);
		CraftingNeeds.Add(EResourceTypes::CopperFragment, 8);
		CraftingNeeds.Add(EResourceTypes::ElectronicComponent, 1);
		CraftingNeeds.Add(EResourceTypes::MetalPlate, 0);
		break;
	}
}

EDamageTypes UArrowInventoryItem::GetArrowType() 
{
	return DamageType;
}

void UArrowInventoryItem::SetWeaponWidget(UWeaponWidget* NewWeaponWidget)
{
	WeaponWidget = NewWeaponWidget;
}
