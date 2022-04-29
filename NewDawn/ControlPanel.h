#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "Enums.h"
#include "ControlPanel.generated.h"

class UPointLightComponent;
class UParticleSystem;

UCLASS()
class NEWDAWN_API AControlPanel : public ATrigger
{
	GENERATED_BODY()

public:
	AControlPanel();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, Category = "Componenti")  UPointLightComponent* Light;
	UPROPERTY(EditAnywhere, Category = "Componenti")  UParticleSystem* LockedSpark;
	UPROPERTY(EditAnywhere, Category = "Componenti")  UParticleSystem* ActiveSpark;

	UFUNCTION()	bool PanelToggle(bool Unlockable = false);
	void UpdateLight();

private:
	UPROPERTY(EditAnywhere, Category = "Variabili") ETriggerState TriggerState;

	const FVector SparkLocation = FVector(0.f, 0.f, 10.f);
	const float LightRadius = 8.f;
	const float LightIntensity = 50000.f;
	const FColor OffColor = FColor::Red;
	const FColor OnColor = FColor::Green;
	const FColor LockedColor = FColor::Blue;
	const FVector LightLocation = FVector(-4.f, 0.f, 8.f);
};
