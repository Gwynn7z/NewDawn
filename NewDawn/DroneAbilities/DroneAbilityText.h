#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DroneAbilityText.generated.h"

class UButton;
class UTextBlock;
class UDroneAbility;
class UDroneActive;

UCLASS()
class NEWDAWN_API UDroneAbilityText : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget)) UButton* AbilityButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* AbilityName;

	UFUNCTION() void SetBoundAbility(UDroneAbility* NewAbility);
	UFUNCTION() void SetInfoPanel(UDroneActive* NewReference);

protected:
	UFUNCTION() void OnAbilityHovered();

	UPROPERTY() UDroneAbility* BoundAbility; //Get infos
	UPROPERTY() UDroneActive* InfoPanel; //Update texts
};
