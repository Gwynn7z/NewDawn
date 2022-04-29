#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

class UTextBlock;
class UVerticalBox;

UCLASS()
class NEWDAWN_API UInteractWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION() void SetInteractText(FString Text);
	UFUNCTION() void AddToLog(FString Text);
	UFUNCTION() void RemoveFromLog();

protected:
	UPROPERTY(meta = (BindWidget)) UTextBlock* Interact;
	UPROPERTY(meta = (BindWidget)) UVerticalBox* Log;
};
