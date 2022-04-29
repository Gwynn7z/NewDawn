#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Structs.h"
#include "FloatingDrone.generated.h"

class USoundCue;
class UMaterialInstanceDynamic;
class UParticleSystemComponent;
class UFloatingDroneAnimInstance;

UCLASS()
class NEWDAWN_API AFloatingDrone : public AEnemy
{
	GENERATED_BODY()

public:
	AFloatingDrone();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animazioni") UFloatingDroneAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, Category = "Variabili")	float Speed;
	UPROPERTY(EditAnywhere, Category = "Variabili")	float InFightRotationSpeed;
	UPROPERTY(EditAnywhere, Category = "Variabili")	float PatrolRotationSpeed;

	UFUNCTION(BlueprintCallable) void StartShooting();

	void EnterCombatMode();
	void ExitCombatMode();

protected:
	virtual void DestroyEnemy() override;

private:
	UFUNCTION() void Shoot();
	UFUNCTION() void PredictShot();
	bool CheckBrokenAccroche(FName AccrocheName);

	FTimerHandle ShootTimer;
	FTimerHandle PredictShootTimer;
	ESide ShootSide;

	float FireRateMin;
	float FireRateMax;

	UPROPERTY() USoundCue* ShootSound;
	UPROPERTY() UMaterialInstanceDynamic* LightMaterial;
	UPROPERTY() UParticleSystem* PredictShoot;
	UPROPERTY() UParticleSystemComponent* CurrentPredictLight;

	const float LightIntensity = 20000.f;
	const float AttenuationRadius = 1000.f;
	const float OuterConeAngle = 60.f;
};
