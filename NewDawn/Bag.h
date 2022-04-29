#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Structs.h"
#include "Bag.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class NEWDAWN_API ABag : public AActor
{
	GENERATED_BODY()
	
public:	
	ABag();
	UPROPERTY(EditAnywhere, Category = "Components") UStaticMeshComponent* BagMesh;
	UPROPERTY(EditAnywhere, Category = "Components") UBoxComponent* TriggerBox;
	FUnownedInventory* GetInventory();
	void AddToInventory(FUnownedInventory NewInventory);

private:
	FUnownedInventory Inventory;
};
