#include "InventoryHUD.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/CanvasPanel.h"
#include "Components/PanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Items/DropWidget.h"
#include "Items/Inventory.h"
#include "Items/BowInventoryItem.h"
#include "Items/ArrowInventoryItem.h"
#include "Items/MissionInventoryItem.h"
#include "Items/ResourcesInventoryItem.h"
#include "Items/UsableInventoryItem.h"
#include "Items/MedkitInventoryItem.h"
#include "Items/TextInventoryWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Fonts/SlateFontInfo.h"

UInventoryHUD::UInventoryHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetAsset(TEXT("/Game/HUD/InventoryText"));
	InventoryWidget = InventoryWidgetAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> DropWidgetAsset(TEXT("/Game/HUD/Drop"));
	DropWidget = DropWidgetAsset.Class;
}

void UInventoryHUD::SetInventory(UInventory* NewInventory)
{
	Inventory = NewInventory;
	WeaponsButton->OnHovered.AddDynamic(this, &UInventoryHUD::ShowBows);
	AmmunitionsButton->OnHovered.AddDynamic(this, &UInventoryHUD::ShowArrows);
	UsablesButton->OnHovered.AddDynamic(this, &UInventoryHUD::ShowUsables);
	ResourcesButton->OnHovered.AddDynamic(this, &UInventoryHUD::ShowResources);
	MissionButton->OnHovered.AddDynamic(this, &UInventoryHUD::ShowMissions);

	UseButton->OnClicked.AddDynamic(this, &UInventoryHUD::Use);
	EquipButton->OnClicked.AddDynamic(this, &UInventoryHUD::Equip);
	EquipButton->OnHovered.AddDynamic(this, &UInventoryHUD::ShowMaterials);
	EquipButton->OnUnhovered.AddDynamic(this, &UInventoryHUD::ShowDescription);
	DropButton->OnClicked.AddDynamic(this, &UInventoryHUD::Drop);

	ClearInfoPanel();
}

void UInventoryHUD::ShowBows()
{
	ClearInfoPanel(true);
	CurrentActive = EInventorySeparations::Bow;
	InventoryBox->ClearChildren();
	if (Inventory != nullptr) {
		TArray<UBowInventoryItem*> BowsArray = Inventory->GetBows();
		for (auto& Item : BowsArray) AddToBox(Item);
	}
}

void UInventoryHUD::ShowArrows()
{
	ClearInfoPanel(true);
	CurrentActive = EInventorySeparations::Arrow;
	InventoryBox->ClearChildren();
	if (Inventory != nullptr) {
		TArray<UArrowInventoryItem*> ArrowsArray = Inventory->GetArrows();
		for (auto& Item : ArrowsArray) AddToBox(Item);
	}
}

void UInventoryHUD::ShowResources()
{
	ClearInfoPanel(true);
	CurrentActive = EInventorySeparations::Resource;
	InventoryBox->ClearChildren();
	if (Inventory != nullptr) {
		TArray<UResourcesInventoryItem*> ResourcesArray = Inventory->GetResources();
		for (auto& Item : ResourcesArray) AddToBox(Item);
	}
}

void UInventoryHUD::ShowUsables()
{
	ClearInfoPanel(true);
	CurrentActive = EInventorySeparations::Usable;
	InventoryBox->ClearChildren();
	if (Inventory != nullptr) {
		TArray<UUsableInventoryItem*> UsablesArray = Inventory->GetUsables();
		for (auto& Item : UsablesArray) AddToBox(Item);
	}
}

void UInventoryHUD::ShowMissions()
{
	ClearInfoPanel(true);
	CurrentActive = EInventorySeparations::Mission;
	InventoryBox->ClearChildren();
	if (Inventory != nullptr) {
		TArray<UMissionInventoryItem*> MissionItemsArray = Inventory->GetMissionItems();
		for(auto &Item : MissionItemsArray) AddToBox(Item);
	}
}

void UInventoryHUD::Use()
{
	if (LastHoveredItem) LastHoveredItem->Use();
}

void UInventoryHUD::Equip()
{
	if (LastHoveredItem) LastHoveredItem->Equip();
}

void UInventoryHUD::Drop()
{
	if (LastHoveredItem) {
		UDropWidget* UMGDrop = CreateWidget<UDropWidget>(GetWorld(), DropWidget);
		UMGDrop->PassItem(LastHoveredItem);
		UMGDrop->SetParentHUD(this);
		UMGDrop->AddToViewport();
	}
}

void UInventoryHUD::AddToBox(UInventoryItem* Item)
{
	UTextInventoryWidget* UMGItem = CreateWidget<UTextInventoryWidget>(GetWorld(), InventoryWidget);
	UMGItem->SetBoundItem(Item);
	UMGItem->SetParentReference(this);
	InventoryBox->AddChild(UMGItem);
	UMGItem->SetPadding(FMargin::FMargin(100.f, 0.f, 0.f, 30.f));
}

void UInventoryHUD::ClearInfoPanel(bool Force)
{
	if (LastHoveredItem != nullptr && !Force) SetInfoPanel(LastHoveredItem);
	else
	{
		ItemImage->SetBrushFromTexture(nullptr);
		Stack->SetText(FText::GetEmpty());
		Name->SetText(FText::GetEmpty());
		Description->SetText(FText::GetEmpty());
		UseButton->SetIsEnabled(false);
		EquipButton->SetIsEnabled(false);
		DropButton->SetIsEnabled(false);
		InfoPanel->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryHUD::SetInfoPanel(UInventoryItem* Item)
{
	if (Item != nullptr)
	{
		if (Item->GetStackNumber() == 0 && (Item->GetDropEnabled() || Item->GetUseEnabled())) {
			LastHoveredItem = nullptr;
			ClearInfoPanel();
		}
		else {
			LastHoveredItem = Item;
			ItemImage->SetBrushFromTexture(Item->GetItemTexture());
			Stack->SetText(Item->GetStackText());
			Name->SetText(Item->GetItemName());
			Description->SetText(Item->GetItemDescription());
			UseButton->SetIsEnabled(Item->GetUseEnabled());
			EquipButton->SetIsEnabled(Item->GetEquipEnabled());
			DropButton->SetIsEnabled(Item->GetDropEnabled()); 
			bIsShowingMaterials ? ShowMaterials() : ShowDescription();
			InfoPanel->SetVisibility(ESlateVisibility::Visible);
			UseText->SetText(Item->GetUseText());
			EquipText->SetText(Item->GetEquipText());
		}
	}
}

void UInventoryHUD::UpdateInventory()
{
	switch (CurrentActive)
	{
	case EInventorySeparations::Bow:
		ShowBows();
		break;
	case EInventorySeparations::Arrow:
		ShowArrows();
		break;
	case EInventorySeparations::Resource:
		ShowResources();
		break;
	case EInventorySeparations::Usable:
		ShowUsables();
		break;
	case EInventorySeparations::Mission:
		ShowMissions();
		break;
	default:
		break;
	}
	if (LastHoveredItem) SetInfoPanel(LastHoveredItem);
}

void UInventoryHUD::ShowMaterials()
{
	if (LastHoveredItem->GetEquipEnabled() && LastHoveredItem->GetEquipText().EqualTo(FText::FromString("Craft"))) {
		FSlateFontInfo Font = Description->Font;
		Font.Size = 22;
		Description->SetFont(Font);
		if (LastHoveredItem->IsA(UArrowInventoryItem::StaticClass())) {
			UArrowInventoryItem* ArrowItem = Cast<UArrowInventoryItem>(LastHoveredItem);
			TMap<EResourceTypes, int32> Needs = ArrowItem->GetCraftingNeeds();
			FString NeedText = "";
			for (auto& Need : Needs) {
				if (Need.Value != 0) {
					NeedText += FString::FromInt(Need.Value) + "/" + FString::FromInt(Inventory->GetQuantityOfResource(Need.Key)) + "  " + ResourceName[Need.Key];
					NeedText += Need.Value > Inventory->GetQuantityOfResource(Need.Key) ? " *\n" : "\n";
				}
			}
			Description->SetText(FText::FromString(NeedText));
			bIsShowingMaterials = true;
		}
	}
}

void UInventoryHUD::ShowDescription()
{
	FSlateFontInfo Font = Description->Font;
	Font.Size = 24;
	Description->SetFont(Font);
	if(LastHoveredItem) Description->SetText(LastHoveredItem->GetItemDescription());
	bIsShowingMaterials = false;
}