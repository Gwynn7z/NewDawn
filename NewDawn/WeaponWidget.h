#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWidget.generated.h"

class UImage;
class UTextBlock;
class UBowInventoryItem;
class UArrowInventoryItem;
class UUsableInventoryItem;
class UCanvasPanel;

UCLASS()
class NEWDAWN_API UWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION() void SetWeaponItem(UBowInventoryItem* NewItem);
	UFUNCTION() void SetArrowItem(UArrowInventoryItem* NewItem);
	UFUNCTION() void UpdateStack();
	UFUNCTION() void HideBowWidget(bool Hide);

	UFUNCTION() void SetShowingUsable(UUsableInventoryItem* NewItem);
	UFUNCTION() void UpdateItemStack();
	UFUNCTION() void HideUsableWidget(bool Hide);
	UFUNCTION() UUsableInventoryItem* GetUsableItem();
	UFUNCTION() UArrowInventoryItem* GetArrowItem();

protected:
	UPROPERTY(meta = (BindWidget)) UImage* WeaponImage;
	UPROPERTY(meta = (BindWidget)) UImage* UsableImage;
	UPROPERTY(meta = (BindWidget)) UImage* ArrowImage;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Stack;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StackItem;

	UPROPERTY(meta = (BindWidget)) UCanvasPanel* BowPanel;
	UPROPERTY(meta = (BindWidget)) UCanvasPanel* ItemPanel;

private:
	UPROPERTY() UArrowInventoryItem* ArrowItem;
	UPROPERTY() UUsableInventoryItem* UsableItem;
	
};
