#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

class UTexture2D;
class AArcher;
class UInventory;

UENUM()
enum class EShowType : uint8 {
	All,
	Stack,
	Nothing
};

UCLASS()
class NEWDAWN_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItem();

	virtual void Use();
	virtual void Equip();

	void SetItemTexture(UTexture2D* ItemTexture);
	void SetItemOwner(AArcher* ItemNewOwner);
	void SetItemLocation(UInventory* NewLocation);
	void SetItemName(FString ItemNewName);
	void SetItemDescription(FString ItemNewDescription);
	void SetShowType(EShowType Type);

	UFUNCTION() UTexture2D* GetItemTexture();
	UFUNCTION() AArcher* GetItemOwner();
	UFUNCTION() UInventory* GetItemLocation();
	FText GetItemName();
	FText GetItemDescription();
	FText GetStackText();
	FText GetUseText();
	FText GetEquipText();

	bool GetUseEnabled();
	bool GetEquipEnabled();
	bool GetDropEnabled();
	
	void AddToStack(int32 Num);
	void RemoveFromStack(int32 Num);
	void FullSlot();
	void EmptySlot();

	int32 GetStackNumber();
	int32 GetSlotCapacity();
	void SetSlotCapacity(int32 NewCapacity);

	void SetMaxOneSlot(bool Set);
	bool GetMaxOneSlot();

protected:
	UPROPERTY() UTexture2D* Texture;
	UPROPERTY() AArcher* ItemOwner;
	UPROPERTY() UInventory* ItemLocation;
	FText ItemName;
	FText ItemDescription;
	FText UseText;
	FText EquipText;
	EShowType ShowType;

	bool UseEnabled;
	bool EquipEnabled;
	bool DropEnabled;

	int32 NumberInSlot;
	int32 MaxNumberPerSlot;
	bool bMaxOneSlot;
};
