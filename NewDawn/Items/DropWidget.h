#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DropWidget.generated.h"

class USlider;
class UTextBlock;
class UButton;
class UInventoryItem;
class UInventoryHUD;

UCLASS()
class NEWDAWN_API UDropWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget)) USlider* DropSlider;
	UPROPERTY(meta = (BindWidget)) UButton* CancelButton;
	UPROPERTY(meta = (BindWidget)) UButton* DropButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Max;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Current;

	UFUNCTION() void PassItem(UInventoryItem* NewItem);
	UFUNCTION() void Drop();
	UFUNCTION() void Cancel();
	UFUNCTION() void SetParentHUD(UInventoryHUD* NewInventoryHUD);

	UFUNCTION() void ValueChanged(float Value);

protected:
	UPROPERTY() UInventoryItem* BoundItem;
	UPROPERTY() UInventoryHUD* InventoryHUD;
};
