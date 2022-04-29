#include "DroneWeaponsWidget.h"
#include "DroneAbilities/DroneAbility.h"
#include "Components/Image.h"

void UDroneWeaponsWidget::HideImages()
{
	RightImage->SetVisibility(ESlateVisibility::Hidden);
	LeftImage->SetVisibility(ESlateVisibility::Hidden);
}

void UDroneWeaponsWidget::SetAbility(ESide Side, UDroneAbility* NewAbility)
{
	if (NewAbility == nullptr)
	{
		switch (Side)
		{
		case ESide::Right:
			RightImage->SetVisibility(ESlateVisibility::Hidden);
			break;
		case ESide::Left:
			LeftImage->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
		return;
	}
	switch (Side)
	{
	case ESide::Right:
		RightImage->SetBrushFromTexture(NewAbility->GetAbilityTexture());
		RightImage->SetVisibility(ESlateVisibility::Visible);
		break;
	case ESide::Left:
		LeftImage->SetBrushFromTexture(NewAbility->GetAbilityTexture());
		LeftImage->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}
