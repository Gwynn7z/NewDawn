#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeInventoryText.generated.h"

class UButton;
class UTextBlock;
class UArcherUpgrade;

UCLASS()
class NEWDAWN_API UUpgradeInventoryText : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION() void BindUpgrade(UArcherUpgrade* NewItem);
	//UFUNCTION() void SetParentReference(UInventoryHUD* NewReference);

protected:
	UFUNCTION() void OnItemClicked();

	UPROPERTY(meta = (BindWidget)) UButton* UpgradeButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* UpgradeName;

	//UPROPERTY() UInventoryItem* BoundItem; //Get infos
	//UPROPERTY() UInventoryHUD* HUDParent; //Update texts
};
