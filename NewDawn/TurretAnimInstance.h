#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TurretAnimInstance.generated.h"

UCLASS()
class NEWDAWN_API UTurretAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTurretAnimInstance();

	FORCEINLINE void SetIsFighting(bool State) { isFighting = State; };
	FORCEINLINE void SetIsOverheaten(bool State) { isOverheaten = State; };
	FORCEINLINE void SetCannonRotation(FRotator Rotation, FRotator OldRotation) { CannonRotation = Rotation; CannonOldRotation = OldRotation; };

	bool IsShooting();
	void Shoot();
	void LastShoot();
	void StopShooting();

protected:
	UPROPERTY(BlueprintReadWrite) bool isFighting = false;
	UPROPERTY(BlueprintReadWrite) bool isOverheaten = false;
	UPROPERTY(BlueprintReadWrite) FRotator CannonRotation;
	UPROPERTY(BlueprintReadWrite) FRotator CannonOldRotation;

	UAnimMontage* ShootMontage;

	//ANIMATION FUNCTIONS
	void PlayMontage(UAnimMontage* Montage, float PlayRate = 1.f);
	void MontageJump(UAnimMontage* Montage, FString Section);
	bool IsMontagePlaying(UAnimMontage* Montage);
	void StopMontage(UAnimMontage* Montage, float Blend);
};
