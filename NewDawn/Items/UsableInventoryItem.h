#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "../Enums.h"
#include "UsableInventoryItem.generated.h"

class UWeaponWidget;

UCLASS()
class NEWDAWN_API UUsableInventoryItem : public UInventoryItem
{
	GENERATED_BODY()
	
public:
	static const int32 MaxUsableSlots = 4;

	UUsableInventoryItem();
	virtual void Use() override;
	virtual void Equip() override;

	void SetUsableType(EUsableTypes NewType);
	EUsableTypes GetUsableType();

	void SetWeaponWidget(UWeaponWidget* NewWeaponWidget);
	void UpdateWeaponWidget();

protected:
	EUsableTypes UsableType;
	UPROPERTY() UWeaponWidget* WeaponWidget;
};
