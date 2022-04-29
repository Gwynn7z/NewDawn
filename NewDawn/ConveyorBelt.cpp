#include "ConveyorBelt.h"
#include "Components/SceneComponent.h"
#include "Enums.h"

AConveyorBelt::AConveyorBelt()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(Root);

	Step1 = CreateDefaultSubobject<USceneComponent>(TEXT("Step1"));
	Step1->SetupAttachment(RootComponent);

	Step2 = CreateDefaultSubobject<USceneComponent>(TEXT("Step2"));
	Step2->SetupAttachment(RootComponent);

	Speed = 1.f;
	StepNum = 1;
}

void AConveyorBelt::BeginPlay()
{
	Super::BeginPlay();
	if (Slider == nullptr) Destroy();
}

void AConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetState() == ESliderState::Active) {
		FVector NextStepLocation;
		if (StepNum == 1) NextStepLocation = Step1->GetComponentLocation();
		else NextStepLocation = Step2->GetComponentLocation();
		FVector CurrentLocation = Slider->GetActorLocation();
		Slider->SetActorLocation(FMath::VInterpTo(CurrentLocation, NextStepLocation, DeltaTime, Speed));
		if (CurrentLocation.Equals(NextStepLocation, 50.f))  StepNum *= -1;
	}
}
