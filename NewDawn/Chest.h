#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Structs.h"
#include "Chest.generated.h"

class UInventory;

UCLASS()
class NEWDAWN_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	AChest();

	UPROPERTY(EditAnywhere, Category = "Components") UStaticMeshComponent* TrialMesh;
	FUnownedInventory* GetInventory();

	UPROPERTY(EditAnywhere, Category = "Inventory") TMap<EResourceTypes, int32> Resources;
	UPROPERTY(EditAnywhere, Category = "Inventory") TMap<EDamageTypes, int32> Ammunitions;
	UPROPERTY(EditAnywhere, Category = "Inventory") TMap<EUsableTypes, int32> Usables;

protected:
	virtual void BeginPlay() override;

private:
	FUnownedInventory Inventory;
};
