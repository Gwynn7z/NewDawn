#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "MissionInventoryItem.generated.h"

UCLASS()
class NEWDAWN_API UMissionInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	static const int32 MaxMissionItemSlots = 10;

	UMissionInventoryItem();

	void SetMissionID(int32 ID);
	int32 GetMissionID();

protected:
	int32 MissionID;
};
