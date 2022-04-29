#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AllyDroneAnimInstance.generated.h"


UCLASS()
class NEWDAWN_API UAllyDroneAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	public:
		UAllyDroneAnimInstance();

		FORCEINLINE void SetIsOpen(bool State) {IsOpen = State; };
		void Shoot();

	protected:
		UPROPERTY(BlueprintReadWrite) bool IsOpen;

		UAnimMontage* ShootMontage;
		void PlayMontage(UAnimMontage* Montage, float PlayRate = 1.f);
};
