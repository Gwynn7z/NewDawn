#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Enums.h"
#include "DroneActive.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UTexture2D;
class UUserWidget;
class UDroneAbility;
class AAllyDrone;

UCLASS()
class NEWDAWN_API UDroneActive : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget)) UButton* EquipSxButton;
	UPROPERTY(meta = (BindWidget)) UButton* EquipDxButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Name;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Description;
	UPROPERTY(meta = (BindWidget)) UImage* LeftImage;
	UPROPERTY(meta = (BindWidget)) UImage* RightImage;
	UPROPERTY(meta = (BindWidget)) UImage* CurrentImage;

	UFUNCTION() void PassDrone(AAllyDrone* NewDrone);
	UFUNCTION() void SaveLastHoveredAbility(UDroneAbility* NewAbility);

	UFUNCTION() void EquipSX();
	UFUNCTION() void EquipDX();
	UFUNCTION() void Reset();

private:
	UPROPERTY() AAllyDrone* Drone;
	UPROPERTY() UDroneAbility* LastHoveredAbility;
	
};
