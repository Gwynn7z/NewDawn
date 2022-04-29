#pragma once
#include "CoreMinimal.h"
#include "Enums.h"

class ABow;
class AItems;

static TMap<EDamageTypes, float> GetVoidDamageMap()
{
	TMap<EDamageTypes, float> NewMap;
	NewMap.Add(EDamageTypes::Physical, 0.f);
	NewMap.Add(EDamageTypes::Tear, 0.f);
	NewMap.Add(EDamageTypes::Fire, 0.f);
	NewMap.Add(EDamageTypes::Ice, 0.f);
	NewMap.Add(EDamageTypes::Electric, 0.f);
	return NewMap;
}

static TMap<EDamageTypes, float> GetDamageMapFromValues(float Physical, float Tear, float Fire, float Ice, float Electric)
{
	TMap<EDamageTypes, float> NewMap;
	NewMap.Add(EDamageTypes::Physical, Physical);
	NewMap.Add(EDamageTypes::Tear, Tear);
	NewMap.Add(EDamageTypes::Fire, Fire);
	NewMap.Add(EDamageTypes::Ice, Ice);
	NewMap.Add(EDamageTypes::Electric, Electric);
	return NewMap;
}

struct FDefensiveStats
{
public:
	TMap<EDamageTypes, float> FullStats;
	TMap<EDamageTypes, float> Stats;

	FDefensiveStats() {
		FullStats = GetVoidDamageMap();
		ResetToFull();
	}

	FDefensiveStats(TMap<EDamageTypes, float> InStats) {
		FullStats = InStats;
		ResetToFull();
	}

	void ResetToFull() {
		Stats = FullStats;
	}

	void ModifyStats(TMap<EDamageTypes, float> InStats, EModifierType Modifier) {
		for (auto& CurrentStat : Stats)
		{
			EDamageTypes CurrentDamageType = CurrentStat.Key;
			switch (Modifier)
			{
			case EModifierType::Increase:
				CurrentStat.Value += InStats[CurrentDamageType];
				if (CurrentStat.Value >= FullStats[CurrentDamageType]) CurrentStat.Value = FullStats[CurrentDamageType];
				break;
			case EModifierType::Decrease:
				if (FullStats[CurrentDamageType] > 0) { //It's 0 for passing values and -1 for immune values
					CurrentStat.Value -= InStats[CurrentDamageType];
					if (CurrentStat.Value <= 0.f) CurrentStat.Value = 0.f;
				}
				break;
			} 
		}
	}

	bool ModifyStat(EDamageTypes StatType, float Value, EModifierType Modifier) {
		if ((Stats[StatType] == FullStats[StatType] && Modifier == EModifierType::Increase) || (Stats[StatType] == 0 && Modifier == EModifierType::Decrease)) return false;
		switch (Modifier)
		{
		case EModifierType::Increase:
			Stats[StatType] += Value;
			if (Stats[StatType] >= FullStats[StatType]) Stats[StatType] = FullStats[StatType];
			break;
		case EModifierType::Decrease:
			Stats[StatType] -= Value;
			if (Stats[StatType] <= 0.f) Stats[StatType] = 0.f;
			break;
		}
		return true;
	}

	void ResetEveryElementalStat() {
		for (auto& Stat : Stats) {
			switch (Stat.Key) {
			case EDamageTypes::Electric:
			case EDamageTypes::Fire:
			case EDamageTypes::Ice:
				Stat.Value = FullStats[Stat.Key];
				break;
			default:
				break;
			}
		}
	}

	void ResetSingleStat(EDamageTypes DamageType) {
		Stats[DamageType] = FullStats[DamageType];
	}

	TArray<EDamageTypes> GetZeroValues() {
		TArray<EDamageTypes> DamageTypes;
		for (auto& Stat : Stats) {
			if (Stat.Value == 0.f && FullStats[Stat.Key] > 0) DamageTypes.Add(Stat.Key);
		}
		return DamageTypes;
	}

	TArray<EDamageTypes> GetIgnoredDamages() {
		TArray<EDamageTypes> IgnoringDamages;
		for (auto& Stat : FullStats) if (Stat.Value == 0.f) IgnoringDamages.Add(Stat.Key);
		return IgnoringDamages;
	}

	TArray<EDamageTypes> GetImmuneDamages() {
		TArray<EDamageTypes> ImmuneDamages;
		for (auto& Stat : FullStats) if (Stat.Value == -1.f) ImmuneDamages.Add(Stat.Key);
		return ImmuneDamages;
	}

	bool IsZeroValue(EDamageTypes DamageType) {
		return Stats[DamageType] == 0.f && FullStats[DamageType] > 0;
	}
};

struct FAggressiveStats
{
public:
	TMap<EDamageTypes, float> DefaultStats;
	TMap<EDamageTypes, float> Stats;

	FAggressiveStats() {
		DefaultStats = GetVoidDamageMap();
		ResetToDefault();
	}

	FAggressiveStats(TMap<EDamageTypes, float> InStats) {
		DefaultStats = InStats;
		ResetToDefault();
	}

	void SetStats(TMap<EDamageTypes, float> InStats) {
		DefaultStats = InStats;
		ResetToDefault();
	}

	void ResetToDefault() {
		Stats = DefaultStats;
	}

	void ModifyStats(TMap<EDamageTypes, float> InStats, EModifierType Modifier) {
		for (auto& CurrentStat : Stats)
		{
			EDamageTypes CurrentDamageType = CurrentStat.Key;
			switch (Modifier)
			{
			case EModifierType::Increase:
				CurrentStat.Value += InStats[CurrentDamageType];
				break;
			case EModifierType::Decrease:
				CurrentStat.Value -= InStats[CurrentDamageType];
				break;
			}
		}
	}
};

struct FSkeletalParts
{
private:
	bool isAttached;
	bool isAWeapon;
	float DamageModifier;
public:
	FDefensiveStats DefensiveStats;
	FAggressiveStats DamageStats;
	FName ConstraintName;
	
	FSkeletalParts() {
		DefensiveStats = FDefensiveStats();
		DamageStats = FAggressiveStats();
		ConstraintName = NAME_None;
		isAttached = false;
		isAWeapon = false;
		DamageModifier = 1.f;
	}

	FSkeletalParts(FDefensiveStats Stats, FName Name) {
		DamageStats = FAggressiveStats();
		DefensiveStats = Stats;
		ConstraintName = Name;
		isAttached = true;
		isAWeapon = false;
		DamageModifier = 1.f;
	}

	FSkeletalParts(TMap<EDamageTypes, float> Stats, FName Name) {
		FSkeletalParts(FDefensiveStats(Stats), Name);
	}

	bool IsAttached() {
		return isAttached;
	}

	void BreakConstraint() {
		isAttached = false;
	}

	bool IsAWeapon() {
		return isAWeapon;
	}

	void SetAsWeapon(TMap<EDamageTypes, float> InDamageStats) {
		isAWeapon = true;
		DamageStats.SetStats(InDamageStats);
	}

	float GetDamageModifier() {
		return DamageModifier;
	}

	void SetDamageModifier(float Value) {
		DamageModifier = Value;
	}
	
};

struct FBowStats
{

private:
	void SetCompatibleArrows() {
		CompatibleArrows.Empty();
		for (auto& Stat : DamageStats.Stats) {
			if (Stat.Value > 0) CompatibleArrows.Add(Stat.Key);
		}
	}

public:
	TArray<EDamageTypes> CompatibleArrows;
	FAggressiveStats DamageStats;
	EBowTypes Type;
	float DrawStrength;
	float DrawSpeed;
	float BraceHeight;
	
	FBowStats() {
		DamageStats = FAggressiveStats();
		DrawStrength = 0.f;
		DrawSpeed = 0.f;
		BraceHeight = 0.f;

		SetCompatibleArrows();
	}

	FBowStats(EBowTypes BowType, TMap<EDamageTypes, float> BowStats, float BowStrength, float BowSpeed, float BowBraceHeight) {
		DamageStats.SetStats(BowStats);
		DrawStrength = BowStrength;
		DrawSpeed = BowSpeed;
		BraceHeight = BowBraceHeight;
		Type = BowType;
		SetCompatibleArrows();
	}
};


struct FUnownedInventory
{
private:
	TMap<EBowTypes, FBowStats> Weapons;
	TMap<EResourceTypes, int32> Resources;
	TMap<EUsableTypes, int32> Usables;
	TMap<EDamageTypes, int32> Ammunitions;
public:
	FUnownedInventory() {
		Weapons.Empty();
		Resources.Empty();
		Usables.Empty();
		Ammunitions.Empty();
	}

	bool IsEmpty() {
		if (Weapons.Num() == 0 && Resources.Num() == 0 && Usables.Num() == 0 && Ammunitions.Num() == 0) return true;
		else return false;
	}

	TMap<EBowTypes, FBowStats> GetWeapons() { return Weapons; }
	TMap<EResourceTypes, int32> GetResources() { return Resources; }
	TMap<EUsableTypes, int32> GetUsables() { return Usables; }
	TMap<EDamageTypes, int32> GetAmmunitions() { return Ammunitions; }

	void AddWeapons(EBowTypes Type, FBowStats Statistics) {
		Weapons.Add(Type, Statistics);
	} 

	void RemoveWeapon(EBowTypes TypeToRemove) {
		for (auto& Weapon : Weapons) {
			if (Weapon.Key == TypeToRemove) {
				Weapons.Remove(TypeToRemove);
				return;
			}
		}
	}

	void UpdateResources(TMap<EResourceTypes, int32> NewResources, EModifierType Modifier) {
		for (auto& Resource : NewResources) {
			if (!Resources.Contains(Resource.Key)) Resources.Add(Resource.Key, 0);
			switch (Modifier) {
			case EModifierType::Decrease:
				Resources[Resource.Key] -= Resource.Value;
				if (Resources[Resource.Key] <= 0) Resources.Remove(Resource.Key);
				break;
			case EModifierType::Increase:
				Resources[Resource.Key] += Resource.Value;
				break;
			}
		}
	}

	void UpdateResource(EResourceTypes Type, int32 Stack, EModifierType Modifier) {
		if (!Resources.Contains(Type)) Resources.Add(Type, 0);
		switch (Modifier) {
			case EModifierType::Decrease:
				Resources[Type] -= Stack;
				if (Resources[Type] <= 0) Resources.Remove(Type);
				break;
			case EModifierType::Increase:
				Resources[Type] += Stack;
				break;
		}
	}

	void UpdateUsables(TMap<EUsableTypes, int32> NewUsables, EModifierType Modifier) {
		for (auto& Usable : NewUsables) {
			if (!Usables.Contains(Usable.Key)) Usables.Add(Usable.Key, 0);
			switch (Modifier) {
			case EModifierType::Decrease:
				Usables[Usable.Key] -= Usable.Value;
				if (Usables[Usable.Key] <= 0) Usables.Remove(Usable.Key);
				break;
			case EModifierType::Increase:
				Usables[Usable.Key] += Usable.Value;
				break;
			}
		}
	}

	void UpdateUsable(EUsableTypes Type, int32 Stack, EModifierType Modifier) {
		if (!Usables.Contains(Type)) Usables.Add(Type, 0);
		switch (Modifier) {
		case EModifierType::Decrease:
			Usables[Type] -= Stack;
			if (Usables[Type] <= 0) Usables.Remove(Type);
			break;
		case EModifierType::Increase:
			Usables[Type] += Stack;
			break;
		}
	}

	void UpdateAmmunitions(TMap<EDamageTypes, int32> NewAmmunitions, EModifierType Modifier) {
		for (auto& Ammunition : NewAmmunitions) {
			if (!Ammunitions.Contains(Ammunition.Key)) Ammunitions.Add(Ammunition.Key, 0);
			switch (Modifier) {
			case EModifierType::Decrease:
				Ammunitions[Ammunition.Key] -= Ammunition.Value;
				if (Ammunitions[Ammunition.Key] <= 0) Ammunitions.Remove(Ammunition.Key);
				break;
			case EModifierType::Increase:
				Ammunitions[Ammunition.Key] += Ammunition.Value;
				break;
			}
		}
	}

	void UpdateAmmunition(EDamageTypes Type, int32 Stack, EModifierType Modifier) {
		if (!Ammunitions.Contains(Type)) Ammunitions.Add(Type, 0);
		switch (Modifier) {
		case EModifierType::Decrease:
			Ammunitions[Type] -= Stack;
			if (Ammunitions[Type] <= 0) Ammunitions.Remove(Type);
			break;
		case EModifierType::Increase:
			Ammunitions[Type] += Stack;
			break;
		}
	}

	void UpdateNewInventory(FUnownedInventory NewInventory, EModifierType Modifier) {
		UpdateAmmunitions(NewInventory.GetAmmunitions(), Modifier);
		UpdateResources(NewInventory.GetResources(), Modifier);
		UpdateUsables(NewInventory.GetUsables(), Modifier);
	}
};