#include "Menu.h"
#include "Archer.h"
#include "AllyDrone.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryHUD.h"
#include "Items/Inventory.h"
#include "DroneAbilities/DroneMenuHUD.h"
#include "Upgrades/UpgradesHUD.h"
#include "UObject/ConstructorHelpers.h"

UMenu::UMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetAsset(TEXT("/Game/HUD/Inventory"));
	InventoryWidgetClass = InventoryWidgetAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> DroneWidgetAsset(TEXT("/Game/HUD/DroneMenu"));
	DroneWidgetClass = DroneWidgetAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> UpgradesWidgetAsset(TEXT("/Game/HUD/Upgrades"));
	UpgradesWidgetClass = UpgradesWidgetAsset.Class;
}

void UMenu::SetInventory(UInventory* NewInventory)
{
	Inventory = NewInventory;

	GeneralButton->OnHovered.AddDynamic(this, &UMenu::ShowGeneral);
	InventoryButton->OnHovered.AddDynamic(this, &UMenu::ShowInventory);
	DroneButton->OnHovered.AddDynamic(this, &UMenu::ShowDrone);
	UpgradesButton->OnHovered.AddDynamic(this, &UMenu::ShowUpgrades);
	OptionsButton->OnHovered.AddDynamic(this, &UMenu::ShowOptions);

	DroneButton->SetIsEnabled(false);
}

void UMenu::SetDrone(AAllyDrone* NewDrone)
{
	Drone = NewDrone;
	DroneButton->SetIsEnabled(true);
}


void UMenu::SetShow(bool Show)
{
	if (Show)
	{
		this->AddToViewport();
		if (Drone) {
			if(Drone->GetIsDown()) DroneButton->SetIsEnabled(false);
			else DroneButton->SetIsEnabled(true);
		}
	}
	else
	{
		MenuPanel->ClearChildren();
		this->RemoveFromViewport();
		if (Inventory) Inventory->SetHUDReference(nullptr);
	}
}

void UMenu::ShowGeneral()
{
	MenuPanel->ClearChildren();
}

void UMenu::ShowInventory()
{
	MenuPanel->ClearChildren();
	InventoryWidget = CreateWidget<UInventoryHUD>(GetWorld(), InventoryWidgetClass);
	InventoryWidget->SetInventory(Inventory);
	if (Inventory) Inventory->SetHUDReference(InventoryWidget);
	UCanvasPanelSlot* PanelSlot = MenuPanel->AddChildToCanvas(InventoryWidget);
	PanelSlot->SetMaximum(FVector2D(1.f, 1.f));
	PanelSlot->SetMinimum(FVector2D(0.f, 0.f));
	PanelSlot->SetOffsets(FMargin::FMargin());
}

void UMenu::ShowDrone()
{
	MenuPanel->ClearChildren();
	DroneWidget = CreateWidget<UDroneMenuHUD>(GetWorld(), DroneWidgetClass);
	DroneWidget->SetDrone(Drone);
	DroneWidget->PassInventory(Inventory);
	UCanvasPanelSlot* PanelSlot = MenuPanel->AddChildToCanvas(DroneWidget);
	PanelSlot->SetMaximum(FVector2D(1.f, 1.f));
	PanelSlot->SetMinimum(FVector2D(0.f, 0.f));
	PanelSlot->SetOffsets(FMargin::FMargin());
}

void UMenu::ShowUpgrades()
{
	MenuPanel->ClearChildren();
	UpgradesWidget = CreateWidget<UUpgradesHUD>(GetWorld(), UpgradesWidgetClass);
	UCanvasPanelSlot* PanelSlot = MenuPanel->AddChildToCanvas(UpgradesWidget);
	PanelSlot->SetMaximum(FVector2D(1.f, 1.f));
	PanelSlot->SetMinimum(FVector2D(0.f, 0.f));
	PanelSlot->SetOffsets(FMargin::FMargin());
}

void UMenu::ShowOptions()
{
	MenuPanel->ClearChildren();
}

void UMenu::UpdateInventory()
{
	if (InventoryWidget) InventoryWidget->UpdateInventory();
}

void UMenu::EnableInventory(bool bEnabled) 
{
	InventoryButton->SetIsEnabled(bEnabled);
}