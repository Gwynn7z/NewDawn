#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FloatingDroneAnimInstance.generated.h"


UCLASS()
class NEWDAWN_API UFloatingDroneAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFloatingDroneAnimInstance();

	FORCEINLINE void SetIsFighting(bool State) { isFighting = State; };

	void ShootDX();
	void ShootSX();

protected:
	UPROPERTY(BlueprintReadWrite) bool isFighting = false;

	UAnimMontage* ShootDXMontage;
	UAnimMontage* ShootSXMontage;

	//ANIMATION FUNCTIONS
	void PlayMontage(UAnimMontage* Montage, float PlayRate = 1.f);
};
