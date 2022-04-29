#include "BowInventoryItem.h"
#include "../Archer.h"
#include "UObject/ConstructorHelpers.h"

UBowInventoryItem::UBowInventoryItem()
{
	SetMaxOneSlot(true);
	SetSlotCapacity(1);
	SetShowType(EShowType::Nothing);
	EquipEnabled = true;

	static ConstructorHelpers::FObjectFinder<UTexture2D> BanukBowAsset(TEXT("Texture2D'/Game/HUD/Textures/BanukBow.BanukBow'"));
	if (BanukBowAsset.Object != NULL) BanukTexture = BanukBowAsset.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> CarjaBowAsset(TEXT("Texture2D'/Game/HUD/Textures/CarjaBow.CarjaBow'"));
	if (CarjaBowAsset.Object != NULL) CarjaTexture = CarjaBowAsset.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> NoraBowAsset(TEXT("Texture2D'/Game/HUD/Textures/NoraBow.NoraBow'"));
	if (NoraBowAsset.Object != NULL) NoraTexture = NoraBowAsset.Object;
}

void UBowInventoryItem::Equip()
{
	GetItemOwner()->UpdateEquippedBow(this);
	Super::Equip();
}

void UBowInventoryItem::SetBowStats(FBowStats NewStats)
{
	BowInfos = NewStats;

	switch (BowInfos.Type) {
	case EBowTypes::Banuk:
		SetItemName("Banuk Bow");
		SetItemDescription("Bow optimized to use elemental arrows");
		SetItemTexture(BanukTexture);
		break;
	case EBowTypes::Carja:
		SetItemName("Carja Bow");
		SetItemDescription("Bow that sacrifices some damage for a faster shooting");
		SetItemTexture(CarjaTexture);
		break;
	case EBowTypes::Nora:
		SetItemName("Nora Bow");
		SetItemDescription("Strong bow, capable of making great damage at the expense of the draw speed");
		SetItemTexture(NoraTexture);
		break;
	}
}

FBowStats UBowInventoryItem::GetBowStats()
{
	return BowInfos;

}