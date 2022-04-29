#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DefensesBars.generated.h"

class UProgressBar;

UCLASS()
class NEWDAWN_API UDefensesBars : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
		void SetMainBarMaxValue(float Value);
	UFUNCTION()
		void SetMainBarValue(float Value);

	UFUNCTION()
		void SetSecondaryBarMaxValue(float Value);
	UFUNCTION()
		void SetSecondaryBarValue(float Value);

	UFUNCTION()
		void SetEnergyBarMaxValue(float Value);
	UFUNCTION()
		void SetEnergyBarValue(float Value);

	UFUNCTION()
		void HideSecondaryBar(bool bHide);
	UFUNCTION()
		void HideEnergyBar(bool bHide);
protected:
	UPROPERTY(meta = (BindWidget))
		UProgressBar* MainBar;
	UPROPERTY(meta = (BindWidget))
		UProgressBar* SecondaryBar;
	UPROPERTY(meta = (BindWidget))
		UProgressBar* EnergyBar;
	
	float MaxMainBar;
	float MaxSecondaryBar;
	float MaxEnergyBar;
};
