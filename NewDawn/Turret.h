#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Structs.h"
#include "Enums.h"
#include "Turret.generated.h"

class USceneComponent;
class USoundCue;
class UTurretAnimInstance;

UCLASS()
class NEWDAWN_API ATurret : public AEnemy
{
	GENERATED_BODY()

public:
	ATurret();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Componenti") USceneComponent* RootComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animazioni") UTurretAnimInstance* AnimInstance;

	UFUNCTION(BlueprintCallable) void ShootLeft();
	UFUNCTION(BlueprintCallable) void ShootRight();
	UFUNCTION(BlueprintCallable) void HeatUp();
	UFUNCTION(BlueprintCallable) void TriggerOverHeat();

	UFUNCTION() void StartShooting();
	UFUNCTION() void StopShooting();

	void SetCannonRotation(FRotator Rotation);
	FVector GetCannonLocation();
	FRotator GetCannonRotation();

protected:
	UFUNCTION() virtual void CheckExceedingStats() override;

private:
	UFUNCTION() void SetFighting(bool isFighting);
	UFUNCTION() void CoolDown();
	void Shoot(FSkeletalParts Side);

	USoundCue* ShootSound;

	int32 HeatValue;
	FTimerHandle OverHeatTimer;
	float OverHeatTimerValue;

	bool BlendControl;
	bool CanShoot;
	bool isOverHeat;

	const float LightIntensity = 20000.f;
	const float AttenuationRadius = 1000.f;
	const float OuterConeAngle = 60.f;

};
