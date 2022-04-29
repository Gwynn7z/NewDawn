#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Items.generated.h"

UCLASS()
class NEWDAWN_API AItems : public AActor
{
	GENERATED_BODY()

public:
	AItems();

	UPROPERTY(EditAnywhere, Category = "Componenti") UStaticMeshComponent* Mesh;
	TTuple<EUsableTypes, int32> GetUsableItem();
	void RemoveFromStack(int32 StackToRemove);

protected:
	TTuple<EUsableTypes, int32> ItemValues;
};
