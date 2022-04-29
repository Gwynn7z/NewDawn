#include "DropWidget.h"
#include "InventoryItem.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "../InventoryHUD.h"
#include "Inventory.h"

void UDropWidget::PassItem(UInventoryItem* NewItem)
{
	BoundItem = NewItem;

	if (BoundItem) {
		int32 StackNumber = BoundItem->GetStackNumber();
		Max->SetText(FText::FromString(FString::FromInt(StackNumber)));
		Current->SetText(FText::FromString(FString::FromInt(DropSlider->GetValue())));
		DropSlider->SetMaxValue(StackNumber);
	}

	DropSlider->OnValueChanged.AddDynamic(this, &UDropWidget::ValueChanged);
	CancelButton->OnClicked.AddDynamic(this, &UDropWidget::Cancel);
	DropButton->OnClicked.AddDynamic(this, &UDropWidget::Drop);
}

void UDropWidget::SetParentHUD(UInventoryHUD* NewInventoryHUD)
{
	InventoryHUD = NewInventoryHUD;
}

void UDropWidget::Cancel()
{
	BoundItem = nullptr;
	this->RemoveFromViewport();
	if (InventoryHUD) InventoryHUD->SetFocus();
}

void UDropWidget::Drop()
{
	UInventory* Inventory = BoundItem->GetItemLocation();
	Inventory->DropItem(BoundItem, DropSlider->GetValue());
	Inventory->UpdateHUD();
	BoundItem = nullptr;
	Inventory = nullptr;
	this->RemoveFromViewport();
	if (InventoryHUD) InventoryHUD->SetFocus();
}

void UDropWidget::ValueChanged(float Value)
{
	Current->SetText(FText::FromString(FString::FromInt(Value)));
}