#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weight.generated.h"

UCLASS()
class NEWDAWN_API AWeight : public AActor
{
	GENERATED_BODY()
public:
	AWeight();

	UPROPERTY(EditAnywhere, Category = "Componenti") UStaticMeshComponent* Mesh;
};