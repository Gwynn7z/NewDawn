#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TargetAnimInstance.generated.h"

UCLASS()
class NEWDAWN_API UTargetAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetIsOpen(bool State) { isOpen = State; };

protected:
	UPROPERTY(BlueprintReadWrite) bool isOpen = false;
};
