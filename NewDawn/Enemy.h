#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enums.h"
#include "Structs.h"
#include "Enemy.generated.h"

class USpotLightComponent;
class AArcher;
class UParticleSystemComponent;

UCLASS()
class NEWDAWN_API AEnemy : public APawn
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Componenti")	USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Componenti")	USpotLightComponent* Radar;

	virtual void GetHit(TMap<EDamageTypes, float> Damage, const FHitResult& Hit);
	virtual void GetHit(TMap<EDamageTypes, float> Damage);

	FORCEINLINE UFUNCTION() void SetTarget(AActor* NewTarget) { Target = NewTarget; };
	FORCEINLINE UFUNCTION() AActor* GetTarget() { return Target; };

	UFUNCTION() void SetState(EEnemyState State, bool bForceState = false);
	FORCEINLINE UFUNCTION() EEnemyState GetState() { return EnemyState; };

	FUnownedInventory* GetLoot();
	UFUNCTION() void ResetAISensing();

protected:
	void BreakConstraint(FName BoneName);
	virtual void CheckExceedingStats();
	void ResetElementalStates();
	virtual void DestroyEnemy();

	UFUNCTION() void StopBurning();
	UFUNCTION() void Burning();
	UFUNCTION() void UnFreeze();
	UFUNCTION() void RecoverFromStun();
	
	FDefensiveStats MainStats;
	TArray<FSkeletalParts> SkeletalComponents;

	AActor* Target;
	EEnemyState EnemyState;
	EElementalState ElemState;
	FUnownedInventory Loot;

	FTimerHandle BurnTimer;
	float BurnCounter;
	void BurnDamage();

	FTimerHandle IceTimer;
	float IceCounter;

	FTimerHandle ShockTimer;
	float ShockCounter;

	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* Fire;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* Ice;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* Electric;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* Sparks;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* ShootSpark;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* BodyHitParticle;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* BoneHitParticle;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystemComponent* ElementalState;
};
