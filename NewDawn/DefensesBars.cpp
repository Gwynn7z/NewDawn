#include "DefensesBars.h"
#include "Components/ProgressBar.h"

void UDefensesBars::SetMainBarMaxValue(float Value)
{
	MaxMainBar = Value;
}

void UDefensesBars::SetMainBarValue(float Value)
{
	MainBar->SetPercent(Value / MaxMainBar);
}

void UDefensesBars::SetSecondaryBarMaxValue(float Value)
{
	MaxSecondaryBar = Value;
}

void UDefensesBars::SetSecondaryBarValue(float Value)
{
	SecondaryBar->SetPercent(Value / MaxSecondaryBar);
}

void UDefensesBars::SetEnergyBarMaxValue(float Value)
{
	MaxEnergyBar = Value;
}

void UDefensesBars::SetEnergyBarValue(float Value)
{
	EnergyBar->SetPercent(Value / MaxEnergyBar);
}

void UDefensesBars::HideSecondaryBar(bool bHide)
{
	ESlateVisibility Visibilty = bHide ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	SecondaryBar->SetVisibility(Visibilty);
}

void UDefensesBars::HideEnergyBar(bool bHide)
{
	ESlateVisibility Visibilty = bHide ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	EnergyBar->SetVisibility(Visibilty);
}