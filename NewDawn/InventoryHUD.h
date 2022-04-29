#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHUD.generated.h"

class UScrollBox;
class UCanvasPanel;
class UButton;
class UTextBlock;
class UInventory;
class UImage;
class UInventoryItem;
class UUserWidget;

UENUM()
enum class EInventorySeparations : uint8 {
	None,
	Bow,
	Arrow,
	Drone,
	Resource,
	Usable,
	Mission
};

UCLASS()
class NEWDAWN_API UInventoryHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UInventoryHUD(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(meta = (BindWidget)) UScrollBox* InventoryBox;
	UPROPERTY(meta = (BindWidget)) UCanvasPanel* InfoPanel;

	UPROPERTY(meta = (BindWidget)) UButton* WeaponsButton;
	UPROPERTY(meta = (BindWidget)) UButton* AmmunitionsButton;
	UPROPERTY(meta = (BindWidget)) UButton* UsablesButton;
	UPROPERTY(meta = (BindWidget)) UButton* ResourcesButton;
	UPROPERTY(meta = (BindWidget)) UButton* MissionButton;

	UPROPERTY(meta = (BindWidget)) UButton* UseButton;
	UPROPERTY(meta = (BindWidget)) UButton* EquipButton;
	UPROPERTY(meta = (BindWidget)) UButton* DropButton;

	UPROPERTY(meta = (BindWidget)) UTextBlock* Stack;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Name;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Description;
	UPROPERTY(meta = (BindWidget)) UTextBlock* UseText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* EquipText;
	UPROPERTY(meta = (BindWidget)) UImage* ItemImage;

	void SetInventory(UInventory* NewInventory);

	UFUNCTION() void ShowBows();
	UFUNCTION() void ShowArrows();
	UFUNCTION() void ShowResources();
	UFUNCTION() void ShowUsables();
	UFUNCTION() void ShowMissions();
	UFUNCTION() void Use();
	UFUNCTION() void Equip();
	UFUNCTION() void ShowMaterials();
	UFUNCTION() void ShowDescription();
	UFUNCTION() void Drop();
	UFUNCTION() void UpdateInventory();
	UFUNCTION() void SetInfoPanel(UInventoryItem* Item);

protected:
	EInventorySeparations CurrentActive = EInventorySeparations::None;
	UFUNCTION() void AddToBox(UInventoryItem* Item);
	UFUNCTION() void ClearInfoPanel(bool Force = false);
	UPROPERTY() UInventory* Inventory;
	UPROPERTY() UInventoryItem* LastHoveredItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> InventoryWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> DropWidget;
private:
	bool bIsShowingMaterials = false;
};
