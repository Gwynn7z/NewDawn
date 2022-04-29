#pragma once

#include "CoreMinimal.h"
#include "ActorSlider.h"
#include "Rotor.generated.h"

class USceneComponent;

UCLASS()
class NEWDAWN_API ARotor : public AActorSlider
{
	GENERATED_BODY()
	
public:
	ARotor();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, Category = "Componenti")  USceneComponent* RootOverride;
	UPROPERTY(EditAnywhere, Category = "Componenti")  UStaticMeshComponent* Rotore;
	UPROPERTY(EditAnywhere, Category = "Variabili") FRotator ClosedDoorRotator = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, Category = "Variabili") FRotator OpenedDoorRotator = FRotator(0.f, 90.f, 0.f);
};

