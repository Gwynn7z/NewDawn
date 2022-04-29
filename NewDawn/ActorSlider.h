#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "ActorSlider.generated.h"

UCLASS()
class NEWDAWN_API AActorSlider : public AActor
{
	GENERATED_BODY()

public:
	AActorSlider();

	UFUNCTION()	void Toggle();
	UFUNCTION()	void Activate();
	UFUNCTION()	void Deactivate();

	FORCEINLINE UFUNCTION()	ESliderState GetState() { return SliderState; };

protected:
	UPROPERTY(EditAnywhere, Category = "Variabili") float Speed;
	UPROPERTY(EditAnywhere, Category = "Variabili")	ESliderState SliderState;
};
