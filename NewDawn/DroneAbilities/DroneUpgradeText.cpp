#include "DroneUpgradeText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "DroneUpgrade.h"
#include "DronePassive.h"

void UDroneUpgradeText::SetBoundUpgrade(UDroneUpgrade* NewUpgrade)
{
	BoundUpgrade = NewUpgrade;

	if (BoundUpgrade) {
		UpgradeName->SetText(BoundUpgrade->GetUpgradeName());
	}
	Button->OnHovered.AddDynamic(this, &UDroneUpgradeText::OnHovered);
	Button->OnClicked.AddDynamic(this, &UDroneUpgradeText::OnClicked);
}

void UDroneUpgradeText::SetInfoPanel(UDronePassive* NewReference)
{
	InfoPanel = NewReference;
}

void UDroneUpgradeText::OnHovered()
{
	if (InfoPanel && BoundUpgrade) InfoPanel->SaveLastHoveredUpgrade(BoundUpgrade);
}

void UDroneUpgradeText::OnClicked()
{
	if (InfoPanel && BoundUpgrade) InfoPanel->RemoveUpgrade(BoundUpgrade);
}