#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Enums.h"
#include "DroneUpgrade.generated.h"

UCLASS()
class NEWDAWN_API UDroneUpgrade : public UObject
{
	GENERATED_BODY()
	
public:
	FText GetUpgradeName();
	FText GetUpgradeDescription();
	EDroneUpgrade GetUpgrade();

	void SetUpgrade(EDroneUpgrade NewUpgrade);

private:
	EDroneUpgrade Upgrade;
	UPROPERTY() FText UpgradeName;
	UPROPERTY() FText UpgradeDescription;
};
