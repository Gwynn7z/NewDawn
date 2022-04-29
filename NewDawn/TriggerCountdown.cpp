#include "TriggerCountdown.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

ATriggerCountdown::ATriggerCountdown()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh->DestroyComponent();

	Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
	SetRootComponent(Text);

	Text->SetTextRenderColor(Colore);
	Text->SetText(Testo);
	Text->SetWorldSize(WordSize);
	Text->SetWorldRotation(FQuat(FRotator(0, 180, 0)));
	Text->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	Text->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

	CountDown = 5;
}

void ATriggerCountdown::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &ATriggerCountdown::UpdateText, 1.0f, true);
}

void ATriggerCountdown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CountDown == 0) {
		ToggleSlider();
		Destroy();
	}
}

void ATriggerCountdown::SetCountdown(int32 CountdownValue)
{
	CountDown = CountdownValue;
}

void ATriggerCountdown::UpdateText()
{
	--CountDown;
	Text->SetText(FText::FromString(FString::FromInt(CountDown)));
}