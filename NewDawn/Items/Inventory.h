#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Enums.h"
#include "../Structs.h"
#include "Inventory.generated.h"

class UBowInventoryItem;
class UArrowInventoryItem;
class UMissionInventoryItem;
class UResourcesInventoryItem;
class UUsableInventoryItem;
class UInventoryItem;
class UInventoryHUD;
class AArcher;

UCLASS()
class NEWDAWN_API UInventory : public UObject
{
	GENERATED_BODY()

public:
	TArray<UInventoryItem*> GetItems();
	void SetOwner(AArcher* NewOwner);
	void SetHUDReference(UInventoryHUD* HUD);
	void UpdateHUD();

	int32 AddArrows(EDamageTypes ArrowType, int32 Number = 1);
	bool RemoveArrows(EDamageTypes ArrowType, int32 Number = 1);
	bool CheckRemoveArrows(EDamageTypes ArrowType, int32 Number = 1);

	int32 AddResources(EResourceTypes ResourceType, int32 Number = 1);
	bool RemoveResources(EResourceTypes ResourceType, int32 Number = 1);
	bool RemoveResources(TMap<EResourceTypes, int32> MapToRemove);
	bool CheckRemoveResource(EResourceTypes ResourceType, int32 Number = 1);
	bool CheckRemoveResources(TMap<EResourceTypes, int32> MapToRemove);

	int32 AddUsables(EUsableTypes UsableType, int32 Number = 1);
	bool RemoveUsables(EUsableTypes UsableType, int32 Number = 1);
	bool CheckRemoveUsables(EUsableTypes UsableType, int32 Number = 1);

	bool AddBow(FBowStats BowStats);
	bool AddMissionItem(int32 MissionID);
	bool RemoveMissionItem(int32 MissionID);

	void RemoveAllItemsOfClass(UClass* Class);
	void DropItem(UInventoryItem* NewItem, int32 Value);

	UFUNCTION() TArray<UBowInventoryItem*> GetBows();
	UFUNCTION() TArray<UArrowInventoryItem*> GetArrows();
	UFUNCTION() TArray<UMissionInventoryItem*> GetMissionItems();
	UFUNCTION() TArray<UResourcesInventoryItem*> GetResources();
	UFUNCTION() TArray<UUsableInventoryItem*> GetUsables();

	UFUNCTION() int32 GetQuantityOfResource(EResourceTypes ResourceType);

	bool IsEmpty();

protected:
	UPROPERTY()	AArcher* InventoryOwner; //Pass to items
	UPROPERTY()	UInventoryHUD* HUDReference; //To update items when used one

	UPROPERTY() TArray<UBowInventoryItem*> Bows;
	UPROPERTY() TArray<UArrowInventoryItem*> Arrows;
	UPROPERTY() TArray<UMissionInventoryItem*> MissionItems;
	UPROPERTY() TArray<UResourcesInventoryItem*> Resources;
	UPROPERTY() TArray<UUsableInventoryItem*> Usables;
	
	UFUNCTION() int32 ProcessItem(UInventoryItem* Item, int32 Number);

private:
	void AddUsableSlot(EUsableTypes Type);
	void AddArrowSlot(EDamageTypes Type);
	void AddResourceSlot(EResourceTypes Type);
};
