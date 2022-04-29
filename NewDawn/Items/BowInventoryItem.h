#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "../Structs.h"
#include "BowInventoryItem.generated.h"

UCLASS()
class NEWDAWN_API UBowInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	static const int32 MaxBowSlots = 3;

	UBowInventoryItem();
	virtual void Equip() override;

	void SetBowStats(FBowStats NewStats);
	FBowStats GetBowStats();

protected:
	FBowStats BowInfos;

	UPROPERTY() UTexture2D* CarjaTexture;
	UPROPERTY() UTexture2D* NoraTexture;
	UPROPERTY() UTexture2D* BanukTexture;
};
