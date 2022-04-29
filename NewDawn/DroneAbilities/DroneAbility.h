#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Structs.h"
#include "../Enums.h"
#include "DroneAbility.generated.h"

class UTexture2D;
class AAllyDrone;

UCLASS()
class NEWDAWN_API UDroneAbility : public UObject
{
	GENERATED_BODY()
public:
	UDroneAbility();
	void EquipRight(AAllyDrone* Drone);
	void EquipLeft(AAllyDrone* Drone);

	UFUNCTION() UTexture2D* GetAbilityTexture();

	void SetDroneAbility(EAllyDroneAbilities NewAbility);
	EAllyDroneAbilities GetDroneAbility();

	void SetAbilityName(FText Name);
	FText GetAbilityName();

	void SetAbilityDescription(FText Description);
	FText GetAbilityDescription();

protected:
	EAllyDroneAbilities DroneAbility;

	UPROPERTY() UTexture2D* Texture;
	UPROPERTY() FText AbilityName;
	UPROPERTY() FText AbilityDescription;
};
