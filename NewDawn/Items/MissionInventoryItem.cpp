#include "MissionInventoryItem.h"

UMissionInventoryItem::UMissionInventoryItem()
{
	SetItemName("Mission Item");
	SetItemDescription("Item necessary to complete a mission");
	SetSlotCapacity(1);
	SetMaxOneSlot(true);
	SetShowType(EShowType::Nothing);
}

void UMissionInventoryItem::SetMissionID(int32 ID)
{
	MissionID = ID;
}

int32 UMissionInventoryItem::GetMissionID()
{
	return MissionID;
}