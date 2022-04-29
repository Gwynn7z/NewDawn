#include "DroneAbilityText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "DroneAbility.h"
#include "DroneActive.h"

void UDroneAbilityText::SetBoundAbility(UDroneAbility* NewAbility)
{
	BoundAbility = NewAbility;

	if (BoundAbility) {
		AbilityName->SetText(BoundAbility->GetAbilityName());
	}
	AbilityButton->OnHovered.AddDynamic(this, &UDroneAbilityText::OnAbilityHovered);
}

void UDroneAbilityText::SetInfoPanel(UDroneActive* NewReference)
{
	InfoPanel = NewReference;
}

void UDroneAbilityText::OnAbilityHovered()
{
	if (InfoPanel && BoundAbility) InfoPanel->SaveLastHoveredAbility(BoundAbility);
}