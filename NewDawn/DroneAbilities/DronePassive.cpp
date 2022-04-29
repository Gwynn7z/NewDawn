#include "DronePassive.h"
#include "DroneUpgrade.h"
#include "DroneUpgradeText.h"
#include "../AllyDrone.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"

UDronePassive::UDronePassive(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> UpgradeWidgetAsset(TEXT("/Game/HUD/DroneUpgradeText"));
	UpgradeWidget = UpgradeWidgetAsset.Class;
}

void UDronePassive::PassDrone(AAllyDrone* NewDrone)
{
	Drone = NewDrone;
	Reset();
	Name->SetVisibility(ESlateVisibility::Hidden);
	Description->SetVisibility(ESlateVisibility::Hidden);
	EquipButton->SetVisibility(ESlateVisibility::Hidden);
	EquipButton->OnClicked.AddDynamic(this, &UDronePassive::Equip);
}

void UDronePassive::SaveLastHoveredUpgrade(UDroneUpgrade* NewAbility)
{
	LastHoveredUpgrade = NewAbility;
	if (LastHoveredUpgrade) {
		Name->SetVisibility(ESlateVisibility::Visible);
		Description->SetVisibility(ESlateVisibility::Visible);
		EquipButton->SetVisibility(ESlateVisibility::Visible);
		Name->SetText(LastHoveredUpgrade->GetUpgradeName());
		Description->SetText(LastHoveredUpgrade->GetUpgradeDescription());
	}
}

void UDronePassive::Reset()
{
	WrapBoxPassive->ClearChildren();
	if (Drone) {
		TArray<UDroneUpgrade*> Slots = Drone->GetUpgradeSlots();
		for (auto& CurrentSlot : Slots) {
			UDroneUpgradeText* UMGItem = CreateWidget<UDroneUpgradeText>(GetWorld(), UpgradeWidget);
			if (CurrentSlot != nullptr) {
				UMGItem->SetBoundUpgrade(CurrentSlot);
				UMGItem->SetInfoPanel(this);
			}
			WrapBoxPassive->AddChildToWrapBox(UMGItem);
		}
	}
}

void UDronePassive::Equip()
{
	if (LastHoveredUpgrade && Drone) {
		Drone->AddToSlot(LastHoveredUpgrade);
		Reset();
	}
}

void UDronePassive::RemoveUpgrade(UDroneUpgrade* Upgrade)
{
	if (Drone) {
		Drone->RemoveFromSlot(Upgrade);
		Reset();
	}
}
