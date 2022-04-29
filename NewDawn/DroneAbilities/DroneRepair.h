#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Enums.h"
#include "DroneRepair.generated.h"

class UButton;
class UTextBlock;
class UProgressBar;
class UInventory;
class AAllyDrone;

UCLASS()
class NEWDAWN_API UDroneRepair : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget)) UButton* RepairButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Advert;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Requirements;
	UPROPERTY(meta = (BindWidget)) UProgressBar* HP;

	UFUNCTION() void Set(AAllyDrone* NewDrone, UInventory* NewInventory);
	UFUNCTION() void Repair();
	UFUNCTION() void SetPanel();

private:
	UPROPERTY() AAllyDrone* Drone;
	UPROPERTY() UInventory* Inventory;
	TMap<EResourceTypes, int32> Needs;
	bool bCanRepair;
};
