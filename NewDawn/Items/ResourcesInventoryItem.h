#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "../Enums.h"
#include "ResourcesInventoryItem.generated.h"

UCLASS()
class NEWDAWN_API UResourcesInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	static const int32 MaxResourceSlots = 50;

	UResourcesInventoryItem();

	void SetResourceType(EResourceTypes NewType);
	EResourceTypes GetResourceType();

protected:
	EResourceTypes ResourceType;

	UTexture2D* CopperTexture;
	UTexture2D* MetalPlateTexture;
	UTexture2D* WoodTexture;
	UTexture2D* WireTexture;
	UTexture2D* ElectronicComponentTexture;
};
