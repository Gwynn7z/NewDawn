#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BowAnimInstance.generated.h"


UCLASS()
class NEWDAWN_API UBowAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetTransformLocation(FVector Value) { DeltaLocation = Value.Y; };
	FORCEINLINE void SetIsDrawing(bool State) { IsDrawing = State; };
	FORCEINLINE void SetBraceHeight(float Value) { BraceHeight = Value; };

protected:
	UPROPERTY(BlueprintReadWrite) float DeltaLocation;
	UPROPERTY(BlueprintReadWrite) bool IsDrawing = false;
	UPROPERTY(BlueprintReadWrite) float BraceHeight = 0.f;
};
