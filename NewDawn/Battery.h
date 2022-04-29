#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Battery.generated.h"

UCLASS()
class NEWDAWN_API ABattery : public AActor
{
	GENERATED_BODY()

public:
	ABattery();

	UPROPERTY(EditAnywhere, Category = "Componenti")  UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Variabili")  float Charge;
	UFUNCTION()  void ParticleEffect();

private:
	UParticleSystem* Spark;
};
