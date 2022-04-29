#pragma once
#include "CoreMinimal.h"

//TYPES ENUMS
UENUM()
enum class EResourceTypes : uint8 {
	CopperFragment,
	Wood,
	Wire,
	MetalPlate,
	ElectronicComponent
};

static TMap<EResourceTypes, FString> ResourceName = {
	{EResourceTypes::CopperFragment, "Copper Fragment"},
	{EResourceTypes::Wood, "Wood"},
	{EResourceTypes::Wire, "Wire"},
	{EResourceTypes::MetalPlate, "Metal Plate"},
	{EResourceTypes::ElectronicComponent, "Electronic Component"}
};

UENUM()
enum class EUsableTypes : uint8 {
	Medkit,
	HealthPotion,
	DamageBooster,
	DrawSpeeder
};

static TMap<EUsableTypes, FString> UsableName = {
	{EUsableTypes::Medkit, "Medkit"},
	{EUsableTypes::HealthPotion, "Health Potion"},
	{EUsableTypes::DamageBooster, "Damage Booster"},
	{EUsableTypes::DrawSpeeder, "Draw Speeder"}
};

UENUM()
enum class EBowTypes : uint8 {
	Carja,
	Nora,
	Banuk
};

static TMap<EBowTypes, FString> BowName = {
	{EBowTypes::Carja, "Carja Bow"},
	{EBowTypes::Nora, "Nora Bow"},
	{EBowTypes::Banuk, "Banuk Bow"}
};

UENUM()
enum class EDamageTypes : uint8 {
	Physical = 0,
	Tear = 1,
	Fire = 2,
	Ice = 3,
	Electric = 4
};

static TMap<EDamageTypes, FString> ArrowName = {
	{EDamageTypes::Physical, "Physical Arrow"},
	{EDamageTypes::Tear, "Tear Arrow"},
	{EDamageTypes::Fire, "Fire Arrow"},
	{EDamageTypes::Ice, "Ice Arrow"},
	{EDamageTypes::Electric, "Electric Arrow"}
};


UENUM()
enum class ESide : uint8 {
	Right,
	Left
};
UENUM()
enum class ECoordinate : uint8 {
	North,
	South,
	West,
	Est
};
UENUM() 
enum class EModifierType : uint8 {
	Increase,
	Decrease
};

//STATUS ENUMS
UENUM()
enum class ESliderState : uint8 {
	Deactive,
	Active,
};
UENUM()
enum class ETriggerState : uint8 {
	On,
	Off,
	Locked
};
UENUM()
enum class ETargetType : uint8 {
	Arrow,
	Projectile
};

//ENEMY'S ENUMS
UENUM()
enum class EEnemyState : uint8 {
	Broken,
	Off,
	Idle,
	Alert,
	Fighting
};
UENUM()
enum class EEnemyIAState : uint8 {
	Off,
	Disabled,
	Enabled,
	Triggered,
	LostSight
};
UENUM()
enum class EElementalState : uint8 {
	None,
	OnFire,
	Frozen,
	Stunned
};


//ALLY DRONE'S ENUM
UENUM()
enum class EAllyDroneIAState : uint8 {
	Piloting,
	Idle,
	ReturningToIdle,
	Fighting,
	Broken,
	Off
};
UENUM()
enum class EAllyDroneState : uint8 {
	Active,
	Disabled,
	Broken
};
UENUM()
enum class EAllyDroneAbilities : uint8 {
	None,
	Shot,
	Electrodes
};
UENUM()
enum class EDroneUpgrade : uint8 {
	EnergyDrain,
	EnergySaver
};
static TMap<EDroneUpgrade, FString> DroneUpgradeName = {
	{EDroneUpgrade::EnergyDrain, "Energy Drain"},
	{EDroneUpgrade::EnergySaver, "Energy Saver"},
};


//ARCHER'S ENUMS
UENUM()
enum class EArcherMovement : int32 {
	Moving = 1,
	Still = 0,
	Flying = -1
};
UENUM()
enum class EArcherSpeed : int32 {
	Fast = 750,
	Normal = 500,
	Slow = 450
};
UENUM(BlueprintType)
enum class EArcherStand : uint8 {
	Standing UMETA(DisplayName = "Standing"),
	Crouched UMETA(DisplayName = "Crouched"),
	Hanging UMETA(DisplayName = "Hanging"),
};
UENUM(BlueprintType)
enum class ELifeState : uint8 {
	Alive UMETA(DisplayName = "Alive"),
	Dead UMETA(DisplayName = "Dead"),
};