#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradesHUD.generated.h"

class UScrollBox;
class UCanvasPanel;
class UButton;
class UTextBlock;
class UUserWidget;

UCLASS()
class NEWDAWN_API UUpgradesHUD : public UUserWidget
{
	GENERATED_BODY()


public:
	//UUpgradesHUD(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(meta = (BindWidget)) UScrollBox* UpgradesBox;
	UPROPERTY(meta = (BindWidget)) UScrollBox* CurrentUpgradesBox;
	UPROPERTY(meta = (BindWidget)) UCanvasPanel* InfoPanel;

	UPROPERTY(meta = (BindWidget)) UButton* AggressiveButton;
	UPROPERTY(meta = (BindWidget)) UButton* DefensiveButton;

	UPROPERTY(meta = (BindWidget)) UButton* EquipButton;

	UPROPERTY(meta = (BindWidget)) UTextBlock* Name;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Description;
	
};
