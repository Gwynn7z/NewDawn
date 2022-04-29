#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "PressurePlate.generated.h"

class UShapeComponent;
class UTextRenderComponent;

UCLASS()
class NEWDAWN_API APressurePlate : public ATrigger
{
	GENERATED_BODY()
public:
	APressurePlate();
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, Category = "Componenti") UShapeComponent* TBox;
	UPROPERTY(EditAnywhere, Category = "Componenti") UTextRenderComponent* Text;

private:
	UFUNCTION() void Overlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void EndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	const FVector MeshScale = FVector(0.5f, 0.5f, 0.5f);
	const FVector TBoxScale = FVector(1.5f, 1.5f, 0.75f);
	const FVector TBoxRelativeLocation = FVector(0.f, 0.f, 10.f);
	const FVector TextLocation = FVector(0.f, 0.f, 20.f);
	const FRotator TextRotation = FRotator(90.f, 180.f, 90.f);

	const float WordSize = 100.f;
	const FColor Colore = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "Variabili")	int32 ItemsToActivate;
	int32 CountingWeights;

};
