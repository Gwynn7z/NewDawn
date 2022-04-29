#include "UpgradeInventoryText.h"
#include "ArcherUpgrade.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UUpgradeInventoryText::BindUpgrade(UArcherUpgrade* NewItem)
{
	/*BoundItem = NewItem;

	if (BoundItem) {
		ItemName->SetText(NewItem->GetItemName());
	}
	ItemButton->OnHovered.AddDynamic(this, &UTextInventoryWidget::OnItemHovered);*/
}
/*
void UTextInventoryWidget::SetParentReference(UInventoryHUD* NewReference)
{
	HUDParent = NewReference;
}
*/
void UUpgradeInventoryText::OnItemClicked()
{
	//if (HUDParent) HUDParent->SetInfoPanel(BoundItem);
}