#include "DroneMenuHUD.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/CanvasPanel.h"
#include "Components/PanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "UObject/ConstructorHelpers.h"
#include "DroneActive.h"
#include "DronePassive.h"
#include "DroneRepair.h"
#include "DroneAbility.h"
#include "DroneUpgrade.h"
#include "DroneAbilityText.h"
#include "DroneUpgradeText.h"
#include "../Items/Inventory.h"
#include "../AllyDrone.h"

UDroneMenuHUD ::UDroneMenuHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ActiveAsset(TEXT("/Game/HUD/DroneActive"));
	ActiveWidgetClass = ActiveAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> PassiveAsset(TEXT("/Game/HUD/DronePassive"));
	PassiveWidgetClass = PassiveAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> RepairAsset(TEXT("/Game/HUD/RepairDrone"));
	RepairWidgetClass = RepairAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> DroneTextAsset(TEXT("/Game/HUD/DroneAbilityText"));
	DroneTextWidgetClass = DroneTextAsset.Class;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> DroneUpgradeTextAsset(TEXT("/Game/HUD/DroneUpgradeText"));
	DroneUpgradeText = DroneUpgradeTextAsset.Class;
}

void UDroneMenuHUD::SetDrone(AAllyDrone* NewDrone)
{
	Drone = NewDrone;
	ActiveButton->OnHovered.AddDynamic(this, &UDroneMenuHUD::ShowActive);
	PassiveButton->OnHovered.AddDynamic(this, &UDroneMenuHUD::ShowPassive);
	RepairButton->OnHovered.AddDynamic(this, &UDroneMenuHUD::ShowRepair);
}

void UDroneMenuHUD::ShowActive()
{
	InfoPanel->ClearChildren();
	WrapBoxDrone->ClearChildren();

	ActiveWidget = CreateWidget<UDroneActive>(GetWorld(), ActiveWidgetClass);

	UCanvasPanelSlot* PanelSlot = InfoPanel->AddChildToCanvas(ActiveWidget);
	PanelSlot->SetMaximum(FVector2D(1.f, 1.f));
	PanelSlot->SetMinimum(FVector2D(0.f, 0.f));
	PanelSlot->SetOffsets(FMargin::FMargin());

	if (Drone) {
		if(ActiveWidget) ActiveWidget->PassDrone(Drone);

		TArray<UDroneAbility*> Abilities = Drone->GetUnlockedAbilities();
		for (auto& DroneAbility : Abilities) AddAbilityToBox(DroneAbility);
	}
}

void UDroneMenuHUD::AddAbilityToBox(UDroneAbility* Ability)
{
	UDroneAbilityText* AbilityWidget = CreateWidget<UDroneAbilityText>(GetWorld(), DroneTextWidgetClass);
	AbilityWidget->SetBoundAbility(Ability);
	AbilityWidget->SetInfoPanel(ActiveWidget);
	WrapBoxDrone->AddChildToWrapBox(AbilityWidget);
	AbilityWidget->SetPadding(FMargin::FMargin(100.f, 0.f, 0.f, 30.f));
}

void UDroneMenuHUD::ShowPassive()
{
	InfoPanel->ClearChildren();
	WrapBoxDrone->ClearChildren();

	PassiveWidget = CreateWidget<UDronePassive>(GetWorld(), PassiveWidgetClass);

	UCanvasPanelSlot* PanelSlot = InfoPanel->AddChildToCanvas(PassiveWidget);
	PanelSlot->SetMaximum(FVector2D(1.f, 1.f));
	PanelSlot->SetMinimum(FVector2D(0.f, 0.f));
	PanelSlot->SetOffsets(FMargin::FMargin());

	if (Drone) {
		if (PassiveWidget) PassiveWidget->PassDrone(Drone);

		TArray<UDroneUpgrade*> Upgrades = Drone->GetUnlockedUpgrades();
		for (auto& DroneUpgrade : Upgrades) AddUpgradeToBox(DroneUpgrade);
	}
}

void UDroneMenuHUD::AddUpgradeToBox(UDroneUpgrade* Upgrade)
{
	UDroneUpgradeText* UpgradeWidget = CreateWidget<UDroneUpgradeText>(GetWorld(), DroneUpgradeText);
	UpgradeWidget->SetBoundUpgrade(Upgrade);
	UpgradeWidget->SetInfoPanel(PassiveWidget);
	WrapBoxDrone->AddChildToWrapBox(UpgradeWidget);
	UpgradeWidget->SetPadding(FMargin::FMargin(100.f, 0.f, 0.f, 30.f));
}

void UDroneMenuHUD::ShowRepair()
{
	InfoPanel->ClearChildren();
	WrapBoxDrone->ClearChildren();

	RepairWidget = CreateWidget<UDroneRepair>(GetWorld(), RepairWidgetClass);
	
	UCanvasPanelSlot* PanelSlot = InfoPanel->AddChildToCanvas(RepairWidget);
	PanelSlot->SetMaximum(FVector2D(1.f, 1.f));
	PanelSlot->SetMinimum(FVector2D(0.f, 0.f));
	PanelSlot->SetOffsets(FMargin::FMargin());

	if(Drone && Inventory && RepairWidget) RepairWidget->Set(Drone, Inventory);
}

void UDroneMenuHUD::PassInventory(UInventory* NewInventory)
{
	Inventory = NewInventory;
}