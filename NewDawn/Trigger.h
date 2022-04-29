#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger.generated.h"

class USceneComponent;
class AActorSlider;

UCLASS()
class NEWDAWN_API ATrigger : public AActor
{
	GENERATED_BODY()

public:
	ATrigger();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Componenti")	UStaticMeshComponent* Mesh;

	UFUNCTION()	void ToggleSlider();
	UFUNCTION()	void ActivateSlider();
	UFUNCTION()	void DeactivateSlider();
	UFUNCTION() void SetSlider(AActorSlider* NewSlider);

protected:
	UPROPERTY(EditAnywhere, Category = "Variabili")	TArray<AActorSlider*> Sliders;
};
