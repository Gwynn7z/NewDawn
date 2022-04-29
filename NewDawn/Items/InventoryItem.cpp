#include "InventoryItem.h"
#include "Inventory.h"

class AArcher;

UInventoryItem::UInventoryItem()
{
	ItemName = FText::FromString("");
	ItemDescription = FText::FromString("");
	UseText = FText::FromString("Use");
	EquipText = FText::FromString("Equip");
	ItemOwner = nullptr;
	Texture = nullptr;
	SetMaxOneSlot(true);
	SetShowType(EShowType::Nothing);
	NumberInSlot = 0;
	MaxNumberPerSlot = 1;
	UseEnabled = false;
	EquipEnabled = false;
	DropEnabled = false;
}

void UInventoryItem::SetItemTexture(UTexture2D* ItemTexture)
{
	Texture = ItemTexture;
}

void UInventoryItem::SetItemName(FString ItemNewName)
{
	ItemName = FText::FromString(ItemNewName);
}

void UInventoryItem::SetItemDescription(FString ItemNewDescription)
{
	ItemDescription = FText::FromString(ItemNewDescription);
}

void UInventoryItem::SetItemOwner(AArcher* ItemNewOwner)
{
	ItemOwner = ItemNewOwner;
}

void UInventoryItem::SetItemLocation(UInventory* NewLocation)
{
	ItemLocation = NewLocation;
}

void UInventoryItem::SetShowType(EShowType Type)
{
	ShowType = Type;
}

void UInventoryItem::AddToStack(int32 Num)
{
	NumberInSlot += Num;
}

void UInventoryItem::RemoveFromStack(int32 Num)
{
	NumberInSlot -= Num;
}

void UInventoryItem::FullSlot()
{
	NumberInSlot = MaxNumberPerSlot;
}

void UInventoryItem::EmptySlot()
{
	NumberInSlot = 0;
}

void UInventoryItem::SetSlotCapacity(int32 NewCapacity)
{
	MaxNumberPerSlot = NewCapacity;
}

void UInventoryItem::SetMaxOneSlot(bool Set)
{
	bMaxOneSlot = Set;
}

void UInventoryItem::Use()
{
	ItemLocation->UpdateHUD();
}

void UInventoryItem::Equip()
{
	ItemLocation->UpdateHUD();
}

UTexture2D* UInventoryItem::GetItemTexture()
{
	return Texture;
}

FText UInventoryItem::GetItemName()
{
	return ItemName;
}

FText UInventoryItem::GetItemDescription()
{
	return ItemDescription;
}

AArcher* UInventoryItem::GetItemOwner()
{
	return ItemOwner;
}

FText UInventoryItem::GetStackText()
{
	FString Stringa = "";
	switch (ShowType)
	{
	case EShowType::All:
		Stringa = FString::Printf(TEXT("%d/%d"), GetStackNumber(), GetSlotCapacity());
		break;
	case EShowType::Stack:
		Stringa  = FString::Printf(TEXT("%d"), GetStackNumber());
		break;
	case EShowType::Nothing:
		Stringa = "-";
		break;
	default:
		break;
	}
	return FText::FromString(Stringa);
}

UInventory* UInventoryItem::GetItemLocation()
{
	return ItemLocation;
}

int32 UInventoryItem::GetStackNumber()
{
	return NumberInSlot;
}

int32 UInventoryItem::GetSlotCapacity()
{
	return MaxNumberPerSlot;
}

bool UInventoryItem::GetMaxOneSlot()
{
	return bMaxOneSlot;
}

bool UInventoryItem::GetUseEnabled() {
	return UseEnabled; 
}

bool UInventoryItem::GetEquipEnabled() {
	return EquipEnabled;
}

bool UInventoryItem::GetDropEnabled() {
	return DropEnabled;
}

FText UInventoryItem::GetUseText() {
	return UseText;
}

FText UInventoryItem::GetEquipText() {
	return EquipText;
}