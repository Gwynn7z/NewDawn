#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "../Enums.h"
#include "ArrowInventoryItem.generated.h"

class UWeaponWidget;

UCLASS()
class NEWDAWN_API UArrowInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	static const int32 MaxArrowSlots = 5;

	UArrowInventoryItem();

	virtual void Equip() override;
	void SetWeaponWidget(UWeaponWidget* NewWeaponWidget);
	void SetArrowType(EDamageTypes NewType);
	FORCEINLINE TMap<EResourceTypes, int32> GetCraftingNeeds() { return CraftingNeeds; };
	EDamageTypes GetArrowType();

protected:
	EDamageTypes DamageType;
	TMap<EResourceTypes, int32> CraftingNeeds;

private:
	UTexture2D* NormalArrowTexture;
	UTexture2D* TearArrowTexture;
	UTexture2D* FireArrowTexture;
	UTexture2D* IceArrowTexture;
	UTexture2D* ElectricArrowTexture;

	UPROPERTY() UWeaponWidget* WeaponWidget;
};
