#include "UsableInventoryItem.h"
#include "../WeaponWidget.h"
#include "../Archer.h"
#include "Inventory.h"

UUsableInventoryItem::UUsableInventoryItem() {
	SetMaxOneSlot(true);
	SetShowType(EShowType::All);
	UseEnabled = true;
	EquipEnabled = true;
	DropEnabled = true;
}

void UUsableInventoryItem::Use()
{
	if (ItemLocation) ItemLocation->RemoveUsables(GetUsableType());
	UpdateWeaponWidget();
	
	Super::Use();
}

void UUsableInventoryItem::UpdateWeaponWidget()
{
	if (WeaponWidget) WeaponWidget->UpdateItemStack();
}

void UUsableInventoryItem::Equip()
{
	GetItemOwner()->UpdateShortcutItem(this);
}

void UUsableInventoryItem::SetUsableType(EUsableTypes NewType)
{
	UsableType = NewType;

	switch (UsableType)
	{
	case EUsableTypes::Medkit:
		SetItemName("Medkit");
		SetItemDescription("Recover some health");
		SetSlotCapacity(6);
		break;
	case EUsableTypes::HealthPotion:
		SetItemName("Health Potion");
		SetItemDescription("Recover a lot of health");
		SetSlotCapacity(3);
		break;
	case EUsableTypes::DamageBooster:
		SetItemName("Damage Booster");
		SetItemDescription("Increase your damage from any source for a few seconds");
		SetSlotCapacity(4);
		break;
	case EUsableTypes::DrawSpeeder:
		SetItemName("Draw Speeder");
		SetItemDescription("Draw your bow faster for a few seconds");
		SetSlotCapacity(4);
		break;
	default:
		break;
	}
}

EUsableTypes UUsableInventoryItem::GetUsableType()
{
	return UsableType;
}

void UUsableInventoryItem::SetWeaponWidget(UWeaponWidget* NewWeaponWidget)
{
	WeaponWidget = NewWeaponWidget;
}
