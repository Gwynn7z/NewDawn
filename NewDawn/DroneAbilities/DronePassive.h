#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DronePassive.generated.h"

class UWrapBox;
class UButton;
class UTextBlock;
class UUserWidget;
class UDroneUpgrade;
class AAllyDrone;

UCLASS()
class NEWDAWN_API UDronePassive : public UUserWidget
{
	GENERATED_BODY()
public:
	UDronePassive(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(meta = (BindWidget)) UButton* EquipButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Name;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Description;
	UPROPERTY(meta = (BindWidget)) UWrapBox* WrapBoxPassive;

	UFUNCTION() void PassDrone(AAllyDrone* NewDrone);
	UFUNCTION() void SaveLastHoveredUpgrade(UDroneUpgrade* NewUpgrade);
	UFUNCTION() void RemoveUpgrade(UDroneUpgrade* Upgrade);

	UFUNCTION() void Equip();
	UFUNCTION() void Reset();

protected:
	UPROPERTY() AAllyDrone* Drone;
	UPROPERTY() UDroneUpgrade* LastHoveredUpgrade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UUserWidget> UpgradeWidget;
	
};
