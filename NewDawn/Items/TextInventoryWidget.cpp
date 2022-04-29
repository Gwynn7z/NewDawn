#include "TextInventoryWidget.h"
#include "InventoryItem.h"
#include "../InventoryHUD.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTextInventoryWidget::SetBoundItem(UInventoryItem* NewItem)
{
	BoundItem = NewItem;

	if (BoundItem) {
		ItemName->SetText(NewItem->GetItemName());
	}
	ItemButton->OnHovered.AddDynamic(this, &UTextInventoryWidget::OnItemHovered);
}

void UTextInventoryWidget::SetParentReference(UInventoryHUD* NewReference)
{
	HUDParent = NewReference;
}

void UTextInventoryWidget::OnItemHovered()
{
	if (HUDParent) HUDParent->SetInfoPanel(BoundItem);
}