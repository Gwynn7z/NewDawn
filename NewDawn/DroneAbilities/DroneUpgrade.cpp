#include "DroneUpgrade.h"

FText UDroneUpgrade::GetUpgradeName()
{
	return UpgradeName;
}

FText UDroneUpgrade::GetUpgradeDescription()
{
	return UpgradeDescription;
}

EDroneUpgrade UDroneUpgrade::GetUpgrade()
{
	return Upgrade;
}

void UDroneUpgrade::SetUpgrade(EDroneUpgrade NewUpgrade)
{
	Upgrade = NewUpgrade;
	UpgradeName = FText::FromString(DroneUpgradeName[Upgrade]);
	switch (Upgrade) {
	case EDroneUpgrade::EnergyDrain:
		UpgradeDescription = FText::FromString("Hold R to convert some health into energy");
		break;
	case EDroneUpgrade::EnergySaver:
		UpgradeDescription = FText::FromString("Energy costs lowered by 20%");
		break;
	}
}