#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DroneUpgradeText.generated.h"

class UButton;
class UTextBlock;
class UDroneUpgrade;
class UDronePassive;

UCLASS()
class NEWDAWN_API UDroneUpgradeText : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget)) UButton* Button;
	UPROPERTY(meta = (BindWidget)) UTextBlock* UpgradeName;

	UFUNCTION() void SetBoundUpgrade(UDroneUpgrade* NewUpgrade);
	UFUNCTION() void SetInfoPanel(UDronePassive* NewReference);

protected:
	UFUNCTION() void OnHovered();
	UFUNCTION() void OnClicked();

	UPROPERTY() UDroneUpgrade* BoundUpgrade; //Get infos
	UPROPERTY() UDronePassive* InfoPanel; //Update texts
};
