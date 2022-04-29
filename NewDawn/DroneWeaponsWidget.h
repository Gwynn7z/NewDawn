#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums.h"
#include "DroneWeaponsWidget.generated.h"

class UImage;
class UDroneAbility;

UCLASS()
class NEWDAWN_API UDroneWeaponsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION() void HideImages();
	UFUNCTION() void SetAbility(ESide Side, UDroneAbility* NewAbility);

protected:
	UPROPERTY(meta = (BindWidget)) UImage* RightImage;
	UPROPERTY(meta = (BindWidget)) UImage* LeftImage;
};
