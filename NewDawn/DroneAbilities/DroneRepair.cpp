#include "DroneRepair.h"
#include "../AllyDrone.h"
#include "Components/Button.h"
#include "../Items/Inventory.h"
#include "../Workbench.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Engine/Engine.h"

void UDroneRepair::Set(AAllyDrone* NewDrone, UInventory* NewInventory)
{
	Drone = NewDrone;
	Inventory = NewInventory;
	RepairButton->OnClicked.AddDynamic(this, &UDroneRepair::Repair);

	SetPanel();
}

void UDroneRepair::Repair()
{
	bool Succeded = Inventory->CheckRemoveResources(Needs);
	if (Succeded) Drone->Heal(30.f);
	SetPanel();
}

void UDroneRepair::SetPanel() 
{
	bCanRepair = true;
	if (Drone && Inventory)
	{
		TTuple<float, float> HPs = Drone->GetHPForBar();
		HP->SetPercent(HPs.Value / HPs.Key);

		if (Drone->IsFullHP()) bCanRepair = false;
		if (!Drone->IsWorkbenchOverlapping()) bCanRepair = false;

		Requirements->SetVisibility(ESlateVisibility::Visible);
		HP->SetVisibility(ESlateVisibility::Visible);

		Needs = Drone->GetRequiredMaterials();
		FString NeedText = "";
		for (auto& Need : Needs) {
			if (Need.Value != 0) {
				NeedText += FString::FromInt(Need.Value) + "/" + FString::FromInt(Inventory->GetQuantityOfResource(Need.Key)) + "  " + ResourceName[Need.Key];
				NeedText += Need.Value > Inventory->GetQuantityOfResource(Need.Key) ? " *\n" : "\n";
				if (Need.Value > Inventory->GetQuantityOfResource(Need.Key)) bCanRepair = false;
			}
		}
		Requirements->SetText(FText::FromString(NeedText));
	}
	else {
		bCanRepair = false;
		Requirements->SetVisibility(ESlateVisibility::Hidden);
		HP->SetVisibility(ESlateVisibility::Hidden);
	}
	RepairButton->SetIsEnabled(bCanRepair);
}