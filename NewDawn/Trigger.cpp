#include "Trigger.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "ActorSlider.h"

ATrigger::ATrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ATrigger::ToggleSlider() {
	for(auto &Slider : Sliders) if (Slider != nullptr) Slider->Toggle();
}

void ATrigger::ActivateSlider() {
	for (auto& Slider : Sliders) if (Slider != nullptr) Slider->Activate();
}

void ATrigger::DeactivateSlider() {
	for (auto& Slider : Sliders) if (Slider != nullptr) Slider->Deactivate();
}

void ATrigger::SetSlider(AActorSlider* NewSlider)
{
	Sliders.Add(NewSlider);
}
