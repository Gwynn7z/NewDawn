#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "TriggerCountdown.generated.h"

class UTextRenderComponent;

UCLASS()
class NEWDAWN_API ATriggerCountdown : public ATrigger
{
	GENERATED_BODY()

public:
	ATriggerCountdown();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Componenti")  UTextRenderComponent* Text;

	UFUNCTION() void SetCountdown(int32 CountdownValue);

private:
	UPROPERTY(EditAnywhere, Category = "Variabili")	int32 CountDown;
	FTimerHandle Timer;
	void UpdateText();

	const float WordSize = 40.f;
	const FColor Colore = FColor::Red;
	const FText Testo = FText::FromString("Countdown");

};
