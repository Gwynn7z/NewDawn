#include "ResourcesInventoryItem.h"
#include "UObject/ConstructorHelpers.h"


UResourcesInventoryItem::UResourcesInventoryItem() {
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> CopperAsset(TEXT("Texture2D'/Game/HUD/Textures/CopperFragment.CopperFragment'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> MetalPlateAsset(TEXT("Texture2D'/Game/HUD/Textures/MetalPlate.MetalPlate'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> WoodAsset(TEXT("Texture2D'/Game/HUD/Textures/Wood.Wood'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> WireAsset(TEXT("Texture2D'/Game/HUD/Textures/Wire.Wire'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ElectronicComponentAsset(TEXT("Texture2D'/Game/HUD/Textures/ElectronicComponent.ElectronicComponent'"));

	CopperTexture = CopperAsset.Object;
	MetalPlateTexture = MetalPlateAsset.Object;
	WoodTexture = WoodAsset.Object;
	WireTexture = WireAsset.Object;
	ElectronicComponentTexture = ElectronicComponentAsset.Object;

	SetMaxOneSlot(false);
	SetShowType(EShowType::All);
	DropEnabled = true;
}

void UResourcesInventoryItem::SetResourceType(EResourceTypes NewType)
{
	ResourceType = NewType;

	switch (ResourceType)
	{
	case EResourceTypes::CopperFragment:
		SetItemName("Copper Fragment");
		SetItemDescription("You can use these fragments to buy stuff from merchants");
		SetShowType(EShowType::Stack);
		SetSlotCapacity(900000);
		SetItemTexture(CopperTexture);
		break;
	case EResourceTypes::Wood:
		SetItemName("Wood");
		SetItemDescription("Common flexible wood, useful for crafting");
		SetSlotCapacity(50);
		SetItemTexture(WoodTexture);
		break;
	case EResourceTypes::Wire:
		SetItemName("Wire");
		SetItemDescription("The link between machine's circuits. These wires are durable and can be also used for other things");
		SetSlotCapacity(60);
		SetItemTexture(WireTexture);
		break;
	case EResourceTypes::MetalPlate:
		SetItemName("Metal Plate");
		SetItemDescription("The plate of drones and machines. It's a thin but durable plate of metal with many uses, such as crafting or repairing");
		SetSlotCapacity(25);
		SetItemTexture(MetalPlateTexture);
		break;
	case EResourceTypes::ElectronicComponent:
		SetItemName("Electronic Component");
		SetItemDescription("Component used to create drones. It is useful to upgrade or replace other parts");
		SetSlotCapacity(20);
		SetItemTexture(ElectronicComponentTexture);
		break;
	default:
		break;
	}
}

EResourceTypes UResourcesInventoryItem::GetResourceType()
{
	return ResourceType;
}

