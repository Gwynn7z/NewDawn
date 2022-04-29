#include "Rotor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

ARotor::ARotor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootOverride = CreateDefaultSubobject<USceneComponent>(TEXT("RootOverride"));
	SetRootComponent(RootOverride);
	Rotore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotore"));
	Rotore->SetupAttachment(RootComponent);

	Speed = 2.f;
}

void ARotor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (GetState() == ESliderState::Deactive) Rotore->SetRelativeRotation(ClosedDoorRotator);
	else  Rotore->SetRelativeRotation(OpenedDoorRotator);
}

void ARotor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator Rotazione = Rotore->GetRelativeRotation();
	if (GetState() == ESliderState::Deactive) Rotore->SetRelativeRotation(FMath::RInterpTo(Rotazione, ClosedDoorRotator, DeltaTime, Speed));
	else Rotore->SetRelativeRotation(FMath::RInterpTo(Rotazione, OpenedDoorRotator, DeltaTime, Speed));
}