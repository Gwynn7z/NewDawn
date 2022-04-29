#pragma once

#include "CoreMinimal.h"
#include "UsableInventoryItem.h"
#include "MedkitInventoryItem.generated.h"

UCLASS()
class NEWDAWN_API UMedkitInventoryItem : public UUsableInventoryItem
{
	GENERATED_BODY()

public:
	UMedkitInventoryItem();

	virtual void Use() override;;

protected:
	float Heal;
};
