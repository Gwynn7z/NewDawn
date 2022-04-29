#include "DroneActive.h"
#include "DroneAbility.h"
#include "../AllyDrone.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UDroneActive::PassDrone(AAllyDrone* NewDrone)
{
	Drone = NewDrone;
	if (Drone) {
		UDroneAbility* LeftAbility = Drone->GetAbility(ESide::Left);
		UDroneAbility* RightAbility = Drone->GetAbility(ESide::Right);
		if (LeftAbility) {
			LeftImage->SetVisibility(ESlateVisibility::Visible);
			LeftImage->SetBrushFromTexture(LeftAbility->GetAbilityTexture());
		}
		else LeftImage->SetVisibility(ESlateVisibility::Hidden);
		
		if (RightAbility) {
			RightImage->SetVisibility(ESlateVisibility::Visible);
			RightImage->SetBrushFromTexture(RightAbility->GetAbilityTexture());
		}
		else RightImage->SetVisibility(ESlateVisibility::Hidden);
	}
	Name->SetText(FText::GetEmpty());
	Name->SetVisibility(ESlateVisibility::Hidden);
	Description->SetText(FText::GetEmpty());
	Description->SetVisibility(ESlateVisibility::Hidden);
	CurrentImage->SetVisibility(ESlateVisibility::Hidden);
	EquipDxButton->SetVisibility(ESlateVisibility::Hidden);
	EquipSxButton->SetVisibility(ESlateVisibility::Hidden);

	EquipSxButton->OnClicked.AddDynamic(this, &UDroneActive::EquipSX);
	EquipDxButton->OnClicked.AddDynamic(this, &UDroneActive::EquipDX);
}

void UDroneActive::SaveLastHoveredAbility(UDroneAbility* NewAbility)
{
	LastHoveredAbility = NewAbility;
	if (LastHoveredAbility) {
		Name->SetVisibility(ESlateVisibility::Visible);
		Description->SetVisibility(ESlateVisibility::Visible);
		CurrentImage->SetVisibility(ESlateVisibility::Visible);
		EquipDxButton->SetVisibility(ESlateVisibility::Visible);
		EquipSxButton->SetVisibility(ESlateVisibility::Visible);
		Name->SetText(LastHoveredAbility->GetAbilityName());
		Description->SetText(LastHoveredAbility->GetAbilityDescription());
		CurrentImage->SetBrushFromTexture(LastHoveredAbility->GetAbilityTexture());
	}
}

void UDroneActive::Reset()
{
	if (Drone) {
		UDroneAbility* LeftAbility = Drone->GetAbility(ESide::Left);
		UDroneAbility* RightAbility = Drone->GetAbility(ESide::Right);
		if (LeftAbility) {
			LeftImage->SetVisibility(ESlateVisibility::Visible);
			LeftImage->SetBrushFromTexture(LeftAbility->GetAbilityTexture());
		}
		else LeftImage->SetVisibility(ESlateVisibility::Hidden);

		if (RightAbility) {
			RightImage->SetVisibility(ESlateVisibility::Visible);
			RightImage->SetBrushFromTexture(RightAbility->GetAbilityTexture());
		}
		else RightImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UDroneActive::EquipSX()
{
	if (LastHoveredAbility && Drone) {
		LastHoveredAbility->EquipLeft(Drone);
		Reset();
	}
}

void UDroneActive::EquipDX()
{
	if (LastHoveredAbility && Drone) {
		LastHoveredAbility->EquipRight(Drone);
		Reset();
	}
}