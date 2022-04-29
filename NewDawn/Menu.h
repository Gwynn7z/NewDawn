#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

class UCanvasPanel;
class UButton;
class UInventory;
class UInventoryHUD;
class UUpgradesHUD;
class UDroneMenuHUD;
class AAllyDrone;

UCLASS()
class NEWDAWN_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UMenu(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(meta = (BindWidget)) UCanvasPanel* MenuPanel;
	UPROPERTY(meta = (BindWidget)) UButton* GeneralButton;
	UPROPERTY(meta = (BindWidget)) UButton* InventoryButton;
	UPROPERTY(meta = (BindWidget)) UButton* DroneButton;
	UPROPERTY(meta = (BindWidget)) UButton* UpgradesButton;
	UPROPERTY(meta = (BindWidget)) UButton* OptionsButton;

	void SetDrone(AAllyDrone* NewDrone);
	void SetInventory(UInventory* NewInventory);
	UFUNCTION(BlueprintCallable) void SetShow(bool Show);
	UFUNCTION() void UpdateInventory();
	void EnableInventory(bool bEnable);

protected:
	UFUNCTION() void ShowGeneral();
	UFUNCTION() void ShowInventory();
	UFUNCTION() void ShowDrone();
	UFUNCTION() void ShowUpgrades();
	UFUNCTION() void ShowOptions();

	UPROPERTY() UInventory* Inventory = nullptr;
	UPROPERTY() AAllyDrone* Drone = nullptr;

	UPROPERTY() UInventoryHUD* InventoryWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY() UDroneMenuHUD* DroneWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> DroneWidgetClass;

	UPROPERTY() UUpgradesHUD* UpgradesWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> UpgradesWidgetClass;
};
