#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextInventoryWidget.generated.h"

class UButton;
class UTextBlock;
class UInventoryItem;
class UInventoryHUD;

UCLASS()
class NEWDAWN_API UTextInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION() void SetBoundItem(UInventoryItem* NewItem);
	UFUNCTION() void SetParentReference(UInventoryHUD* NewReference);

protected:
	UFUNCTION() void OnItemHovered();

	UPROPERTY(meta = (BindWidget)) UButton* ItemButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* ItemName;

	UPROPERTY() UInventoryItem* BoundItem; //Get infos
	UPROPERTY() UInventoryHUD* HUDParent; //Update texts
};
