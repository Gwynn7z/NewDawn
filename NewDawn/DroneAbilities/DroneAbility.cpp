#include "DroneAbility.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "../AllyDrone.h"
#include "../Archer.h"

UDroneAbility::UDroneAbility()
{
	SetAbilityName(FText::GetEmpty());
	SetAbilityDescription(FText::GetEmpty());
	SetDroneAbility(EAllyDroneAbilities::None);
}

void UDroneAbility::EquipLeft(AAllyDrone* Drone)
{
	if (Drone != nullptr)
	{
		Drone->SetAbility(ESide::Left, this);
	}
}

void UDroneAbility::EquipRight(AAllyDrone* Drone)
{
	if (Drone != nullptr)
	{
		Drone->SetAbility(ESide::Right, this);
	}
}

void UDroneAbility::SetDroneAbility(EAllyDroneAbilities NewAbility)
{
	DroneAbility = NewAbility;
}

EAllyDroneAbilities UDroneAbility::GetDroneAbility()
{
	return DroneAbility;
}

void UDroneAbility::SetAbilityName(FText Name)
{
	AbilityName = Name;
}

void UDroneAbility::SetAbilityDescription(FText Description)
{
	AbilityDescription = Description;
}

FText UDroneAbility::GetAbilityName() {
	return AbilityName;
}

FText UDroneAbility::GetAbilityDescription() {
	return AbilityDescription;
}

UTexture2D* UDroneAbility::GetAbilityTexture() {
	return Texture;
}