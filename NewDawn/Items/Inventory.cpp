#include "Inventory.h"
#include "ArrowInventoryItem.h"
#include "BowInventoryItem.h"
#include "InventoryItem.h"
#include "MissionInventoryItem.h"
#include "ResourcesInventoryItem.h"
#include "UsableInventoryItem.h"
#include "MedkitInventoryItem.h"
#include "../Enums.h"
#include "../Archer.h"
#include "../InventoryHUD.h"

void UInventory::SetOwner(AArcher* NewOwner)
{
    InventoryOwner = NewOwner;
}

void UInventory::SetHUDReference(UInventoryHUD* HUD)
{
    HUDReference = HUD;
}

void UInventory::UpdateHUD()
{
    if (HUDReference != nullptr)  HUDReference->UpdateInventory();
}

int32 UInventory::ProcessItem(UInventoryItem* Item, int32 Number)
{
    if ((Item->GetStackNumber() + Number) > Item->GetSlotCapacity()) {
        int32 ExceededItems = Number + Item->GetStackNumber() - Item->GetSlotCapacity();
        Item->FullSlot();
        return ExceededItems;
    }
    else {
        Item->AddToStack(Number);
        return 0;
    }
}

void UInventory::DropItem(UInventoryItem* NewItem, int32 Value)
{
    FUnownedInventory DropInventory = FUnownedInventory();
    if (NewItem->IsA(UResourcesInventoryItem::StaticClass()))
    {
        UResourcesInventoryItem* Resource = Cast<UResourcesInventoryItem>(NewItem);
        RemoveResources(Resource->GetResourceType(), Value);
        DropInventory.UpdateResource(Resource->GetResourceType(), Value, EModifierType::Increase);
    }
    else if (NewItem->IsA(UUsableInventoryItem::StaticClass()))
    {
        UUsableInventoryItem* Usable = Cast<UUsableInventoryItem>(NewItem);
        RemoveUsables(Usable->GetUsableType(), Value);
        Usable->UpdateWeaponWidget();
        DropInventory.UpdateUsable(Usable->GetUsableType(), Value, EModifierType::Increase);
    }
    else return;
    InventoryOwner->InventoryDrop(DropInventory);
}
//-----------------------------------------------------------------------------------------------------------

void UInventory::AddArrowSlot(EDamageTypes Type)
{
    UArrowInventoryItem* NewSlot = NewObject<UArrowInventoryItem>();
    NewSlot->SetItemOwner(InventoryOwner);
    NewSlot->SetItemLocation(this);
    NewSlot->SetArrowType(Type);
    Arrows.Add(NewSlot);
}

int32 UInventory::AddArrows(EDamageTypes ArrowType, int32 Number) {
    for (int32 i = Arrows.Num() - 1; i >= 0; i--) {
        UArrowInventoryItem* Slot = Arrows[i];
        if (Slot->GetArrowType() == ArrowType) {
            int32 ExceededArrows = ProcessItem(Slot, Number);
            if (ExceededArrows > 0)
            {
                if (!Slot->GetMaxOneSlot())
                {
                    if (Arrows.Num() >= UArrowInventoryItem::MaxArrowSlots) return ExceededArrows;
                    AddArrowSlot(ArrowType);
                    return AddArrows(ArrowType, ExceededArrows);
                }
                else return ExceededArrows;
            }
            else return 0;
        }
    }
    if (Arrows.Num() >= UArrowInventoryItem::MaxArrowSlots) return Number;
    AddArrowSlot(ArrowType);
    return AddArrows(ArrowType, Number);
}


bool UInventory::RemoveArrows(EDamageTypes ArrowType, int32 Number) {
    for (int32 i = Arrows.Num() - 1; i >= 0; i--) {
        UArrowInventoryItem* Slot = Arrows[i];
        if (Slot->GetArrowType() == ArrowType) {
            if (Slot->GetStackNumber() <= 0) { //Non succede ma non se sa mai
                Arrows.Remove(Slot);
                return RemoveArrows(ArrowType, Number);
            }
            else if (Slot->GetStackNumber() - Number <= 0) { //Ritorna true se almeno una � stata rimossa, quindi la ricorsiva non viene ritornata
                Slot->EmptySlot();
                int32 ExceededArrows = Number - Slot->GetStackNumber();
                Arrows.Remove(Slot);
                if(ExceededArrows != 0) RemoveArrows(ArrowType, ExceededArrows);
            }
            else Slot->RemoveFromStack(Number);

            return true;
        }
    }
    return false;
}

bool UInventory::CheckRemoveArrows(EDamageTypes ArrowType, int32 Number) {
    int32 NumberOfItems = 0;
    for (auto& Slot : Arrows) {
        NumberOfItems += Slot->GetStackNumber();
        if (NumberOfItems >= Number) break;
    }

    if (NumberOfItems >= Number) return RemoveArrows(ArrowType, Number);
    else return false;
}

//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------

void UInventory::AddResourceSlot(EResourceTypes Type)
{
    UResourcesInventoryItem* NewSlot = NewObject<UResourcesInventoryItem>();
    NewSlot->SetItemOwner(InventoryOwner);
    NewSlot->SetItemLocation(this);
    NewSlot->SetResourceType(Type);
    Resources.Add(NewSlot);
}

int32 UInventory::AddResources(EResourceTypes ResourceType, int32 Number) {
    for (int32 i = Resources.Num() - 1; i >= 0; i--) {
        UResourcesInventoryItem* Slot = Resources[i];
        if (Slot->GetResourceType() == ResourceType) {
            int32 ExceededResources = ProcessItem(Slot, Number);

            if (ExceededResources > 0)
            {
                if (!Slot->GetMaxOneSlot())
                {
                    if (Resources.Num() >= UResourcesInventoryItem::MaxResourceSlots) return ExceededResources;
                    AddResourceSlot(ResourceType);
                    return AddResources(ResourceType, ExceededResources);
                }
                else return ExceededResources;
            }
            else return 0;
        }
    }
    if (Resources.Num() >= UResourcesInventoryItem::MaxResourceSlots) return Number;
    AddResourceSlot(ResourceType);
    return AddResources(ResourceType, Number);
}


bool UInventory::RemoveResources(EResourceTypes ResourceType, int32 Number) {
    for (int32 i = Resources.Num() - 1; i >= 0; i--) {
        UResourcesInventoryItem* Slot = Resources[i];
        if (Slot->GetResourceType() == ResourceType) {
            if (Slot->GetStackNumber() <= 0) { //Non succede ma non se sa mai
                Resources.Remove(Slot);
                return RemoveResources(ResourceType, Number);
            }
            else if (Slot->GetStackNumber() - Number <= 0) { //Ritorna true se almeno una � stata rimossa, quindi la ricorsiva non viene ritornata
                Slot->EmptySlot();
                int32 ExceededResources = Number - Slot->GetStackNumber();
                Resources.Remove(Slot);
                if(ExceededResources != 0) RemoveResources(ResourceType, ExceededResources);
            }
            else Slot->RemoveFromStack(Number);

            return true;
        }
    }
    return false;
}

bool UInventory::RemoveResources(TMap<EResourceTypes, int32> MapToRemove)
{
    bool EverythingRemoved = true;
    for (auto& Element : MapToRemove) {
        bool Removed = RemoveResources(Element.Key, Element.Value);
        EverythingRemoved = EverythingRemoved && Removed;
   }
    return EverythingRemoved;
}

bool UInventory::CheckRemoveResource(EResourceTypes ResourceType, int32 Number) {
    int32 NumberOfItems = 0;
    for (auto& Slot : Resources){
        if (Slot->GetResourceType() == ResourceType) {
            NumberOfItems += Slot->GetStackNumber();
        }
        if (NumberOfItems >= Number) break;
    }

    if(NumberOfItems >= Number) return RemoveResources(ResourceType, Number);
    else return false;
}

bool UInventory::CheckRemoveResources(TMap<EResourceTypes, int32> MapToRemove)
{
    for (auto& Element : MapToRemove) {
        int32 NumberOfItems = 0;
        for (auto& Slot : Resources) {
            if (Slot->GetResourceType() == Element.Key) {
                NumberOfItems += Slot->GetStackNumber();
            }
            if (NumberOfItems >= Element.Value) break;
        }
        if (NumberOfItems < Element.Value) return false;
    }
    for (auto& Element : MapToRemove) {
        RemoveResources(Element.Key, Element.Value);
    }
    return true;
}

//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------

void UInventory::AddUsableSlot(EUsableTypes Type)
{
    UUsableInventoryItem* NewSlot = nullptr;
    switch (Type)
    {
    case EUsableTypes::Medkit:
        NewSlot = NewObject<UMedkitInventoryItem>();
        break;
    case EUsableTypes::HealthPotion:
        break;
    case EUsableTypes::DamageBooster:
        break;
    case EUsableTypes::DrawSpeeder:
        break;
    default:
        break;
    }

    if (NewSlot)
    {
        NewSlot->SetItemOwner(InventoryOwner);
        NewSlot->SetItemLocation(this);
        NewSlot->SetUsableType(Type);
        Usables.Add(NewSlot);
    }
}

int32 UInventory::AddUsables(EUsableTypes UsableType, int32 Number) {
    for (int32 i = Usables.Num() - 1; i >= 0; i--) {
        UUsableInventoryItem* Slot = Usables[i];
        if (Slot->GetUsableType() == UsableType) {
            int32 ExceededUsables = ProcessItem(Slot, Number);

            if (ExceededUsables > 0)
            {
                if (!Slot->GetMaxOneSlot())
                {
                    if (Usables.Num() >= UUsableInventoryItem::MaxUsableSlots) return ExceededUsables;
                    AddUsableSlot(UsableType);
                    return AddUsables(UsableType, ExceededUsables);
                }
                else return ExceededUsables;
            }
            else return 0;
        }
    }
    if (Usables.Num() >= UUsableInventoryItem::MaxUsableSlots) return Number;
    AddUsableSlot(UsableType);
    return AddUsables(UsableType, Number);
}


bool UInventory::RemoveUsables(EUsableTypes UsableType, int32 Number) {
    for (int32 i = Usables.Num() - 1; i >= 0; i--) {
        UUsableInventoryItem* Slot = Usables[i];
        if (Slot->GetUsableType() == UsableType) {
            if (Slot->GetStackNumber() <= 0) { //Non succede ma non se sa mai
                Usables.Remove(Slot);
                return RemoveUsables(UsableType, Number);
            }
            else if (Slot->GetStackNumber() - Number <= 0) { //Ritorna true se almeno una � stata rimossa, quindi la ricorsiva non viene ritornata
                Slot->EmptySlot();
                int32 ExceededUsables = Number - Slot->GetStackNumber();
                Usables.Remove(Slot);
                if(ExceededUsables != 0) RemoveUsables(UsableType, ExceededUsables);
            }
            else Slot->RemoveFromStack(Number);

            return true;
        }
    }
    return false;
}

bool UInventory::CheckRemoveUsables(EUsableTypes UsableType, int32 Number) {
    int32 NumberOfItems = 0;
    for (auto& Slot : Usables) {
        NumberOfItems += Slot->GetStackNumber();
        if (NumberOfItems >= Number) break;
    }

    if(NumberOfItems >= Number) return RemoveUsables(UsableType, Number);
    else return false;
}

//-----------------------------------------------------------------------------------------------------------

bool UInventory::AddBow(FBowStats BowStats)
{
    if (Bows.Num() < UBowInventoryItem::MaxBowSlots)
    {
        UBowInventoryItem* NewBow = NewObject<UBowInventoryItem>();
        NewBow->SetBowStats(BowStats);
        NewBow->SetItemOwner(InventoryOwner);
        NewBow->SetItemLocation(this);
        NewBow->FullSlot();
        Bows.Add(NewBow);
        return true;
    }
    else return false;
}

bool UInventory::AddMissionItem(int32 MissionID)
{
    if (MissionItems.Num() < UBowInventoryItem::MaxBowSlots)
    {
        UMissionInventoryItem* NewMissionItem = NewObject<UMissionInventoryItem>();
        NewMissionItem->SetMissionID(MissionID);
        NewMissionItem->SetItemOwner(InventoryOwner);
        NewMissionItem->SetItemLocation(this);
        NewMissionItem->FullSlot();
        MissionItems.Add(NewMissionItem);
        return true;
    }
    else return false;
}

bool UInventory::RemoveMissionItem(int32 MissionID)
{
    for (auto& Item : MissionItems) {
        if (Item->GetMissionID() == MissionID) {
            MissionItems.Remove(Item);
            return true;
        }
    }
    return false;
}

void UInventory::RemoveAllItemsOfClass(UClass* Class)
{
    if      (Class == UArrowInventoryItem::StaticClass()) Arrows.Empty();
    else if (Class == UBowInventoryItem::StaticClass()) Bows.Empty();
    else if (Class == UMissionInventoryItem::StaticClass()) MissionItems.Empty();
    else if (Class == UUsableInventoryItem::StaticClass()) Usables.Empty();
    else if (Class == UResourcesInventoryItem::StaticClass()) Resources.Empty();
}

TArray<UBowInventoryItem*> UInventory::GetBows()
{
    return Bows;
}

TArray<UArrowInventoryItem*> UInventory::GetArrows()
{
    return Arrows;
}

TArray<UMissionInventoryItem*> UInventory::GetMissionItems()
{
    return MissionItems;
}

TArray<UResourcesInventoryItem*> UInventory::GetResources()
{
    return Resources;
}

TArray<UUsableInventoryItem*> UInventory::GetUsables()
{
    return Usables;
}

int32 UInventory::GetQuantityOfResource(EResourceTypes ResourceType)
{
    int32 Quantity = 0;
    for (int32 i = 0; i < Resources.Num(); i++) {
        if (Resources[i]->GetResourceType() == ResourceType) Quantity += Resources[i]->GetStackNumber();
    }
    return Quantity;
}

TArray<UInventoryItem*> UInventory::GetItems()
{
    TArray<UInventoryItem*> Items;

    for (auto& Arrow : Arrows) Items.Add(Arrow);
    for (auto& Bow : Bows) Items.Add(Bow);
    for (auto& MissionItem : MissionItems) Items.Add(MissionItem);
    for (auto& Usable : Usables) Items.Add(Usable);
    for (auto& Resource : Resources) Items.Add(Resource);

    return Items;
}

bool UInventory::IsEmpty()
{
    if (GetItems().Num() == 0) return true;
    else return false;
}