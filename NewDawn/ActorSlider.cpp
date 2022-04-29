#include "ActorSlider.h"

AActorSlider::AActorSlider()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AActorSlider::Toggle()
{
	switch (SliderState)
	{
	case ESliderState::Deactive:
		SliderState = ESliderState::Active;
		break;
	case ESliderState::Active:
		SliderState = ESliderState::Deactive;
		break;
	default:
		break;
	}
}

void AActorSlider::Activate()
{
	SliderState = ESliderState::Active;
}

void AActorSlider::Deactivate()
{
	SliderState = ESliderState::Deactive;
}