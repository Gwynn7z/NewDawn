#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pathfinder.generated.h"

UCLASS()
class NEWDAWN_API APathfinder : public AActor
{
	GENERATED_BODY()

public:
	APathfinder();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere) class UShapeComponent* Box;
	UPROPERTY(BlueprintReadWrite) TArray<FVector> Location;

protected:
	virtual void BeginPlay() override;

private:
	FVector Dimensions;
};
