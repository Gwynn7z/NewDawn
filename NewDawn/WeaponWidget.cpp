#include "WeaponWidget.h"
#include "Items/BowInventoryItem.h"
#include "Items/ArrowInventoryItem.h"
#include "Items/UsableInventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"


void UWeaponWidget::SetWeaponItem(UBowInventoryItem* NewItem)
{
	WeaponImage->SetBrushFromTexture(NewItem->GetItemTexture());
}

void UWeaponWidget::SetArrowItem(UArrowInventoryItem* NewItem)
{
	ArrowImage->SetBrushFromTexture(NewItem->GetItemTexture());
	ArrowItem = NewItem;
	UpdateStack();
}

void UWeaponWidget::UpdateStack()
{
	if(ArrowItem) Stack->SetText(ArrowItem->GetStackText());
}

void UWeaponWidget::HideBowWidget(bool Hide)
{
	ESlateVisibility bVisibility = Hide ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	BowPanel->SetVisibility(bVisibility);
}

void UWeaponWidget::SetShowingUsable(UUsableInventoryItem* NewItem)
{
	UsableItem = NewItem;
	if (NewItem) {
		UsableImage->SetBrushFromTexture(NewItem->GetItemTexture());
		UpdateItemStack();
	}
}

void UWeaponWidget::UpdateItemStack()
{
	if (UsableItem && UsableItem->GetStackNumber() > 0) StackItem->SetText(FText::FromString(FString::FromInt(UsableItem->GetStackNumber())));
	else {
		SetShowingUsable(nullptr);
		HideUsableWidget(true);
	}
}

void UWeaponWidget::HideUsableWidget(bool Hide)
{
	ESlateVisibility bVisibility = Hide ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	ItemPanel->SetVisibility(bVisibility);
}

UUsableInventoryItem* UWeaponWidget::GetUsableItem()
{
	return UsableItem;
}

UArrowInventoryItem* UWeaponWidget::GetArrowItem()
{
	return ArrowItem;
}