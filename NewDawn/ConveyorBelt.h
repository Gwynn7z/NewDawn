#pragma once

#include "CoreMinimal.h"
#include "ActorSlider.h"
#include "ConveyorBelt.generated.h"

class USceneComponent;

UCLASS()
class NEWDAWN_API AConveyorBelt : public AActorSlider
{
	GENERATED_BODY()
	
public:
	AConveyorBelt();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Componenti") USceneComponent* Root;
	UPROPERTY(EditAnywhere, Category = "Componenti") USceneComponent* Step1;
	UPROPERTY(EditAnywhere, Category = "Componenti") USceneComponent* Step2;
	UPROPERTY(EditAnywhere, Category = "Variabili")  AActor* Slider;

private:
	int32 StepNum;
};
