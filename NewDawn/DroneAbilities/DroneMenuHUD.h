#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DroneMenuHUD.generated.h"

class UWrapBox;
class UCanvasPanel;
class UButton;
class UInventory;
class UUserWidget;
class AAllyDrone;
class UDroneActive;
class UDronePassive;
class UDroneRepair;

UCLASS()
class NEWDAWN_API UDroneMenuHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UDroneMenuHUD(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(meta = (BindWidget)) UWrapBox* WrapBoxDrone;
	UPROPERTY(meta = (BindWidget)) UCanvasPanel* InfoPanel;

	UPROPERTY(meta = (BindWidget)) UButton* ActiveButton;
	UPROPERTY(meta = (BindWidget)) UButton* PassiveButton;
	UPROPERTY(meta = (BindWidget)) UButton* RepairButton;

	UFUNCTION() void ShowActive();
	UFUNCTION() void ShowPassive();
	UFUNCTION() void ShowRepair();

	UFUNCTION() void SetDrone(AAllyDrone* NewDrone);
	UFUNCTION() void PassInventory(UInventory* NewInventory);

protected:
	UPROPERTY() AAllyDrone* Drone;
	UPROPERTY() UInventory* Inventory;
	UPROPERTY() UDronePassive* PassiveWidget;
	UPROPERTY() UDroneActive* ActiveWidget;
	UPROPERTY() UDroneRepair* RepairWidget;

	UFUNCTION() void AddAbilityToBox(UDroneAbility* Ability);
	UFUNCTION() void AddUpgradeToBox(UDroneUpgrade* Upgrade);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> ActiveWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> PassiveWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> RepairWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> DroneTextWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> DroneUpgradeText;
};
