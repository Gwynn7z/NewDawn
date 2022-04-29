#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums.h"
#include "ArcherAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ELocomotion : uint8 {
	Idle UMETA(DisplayName = "Idle"),
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
};

UCLASS()
class NEWDAWN_API UArcherAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UArcherAnimInstance();

	FORCEINLINE void SetPitchValue(float Value) { PitchValue = Value; };
	FORCEINLINE void SetIsAiming(bool State) { IsAiming = State; };
	FORCEINLINE void SetIsDrawing(bool State) { IsDrawing = State; };
	FORCEINLINE void SetIsBowBack(bool State) { IsBowBack = State; };

	FORCEINLINE void SetLocomotion(ELocomotion NewLocomotion) { Locomotion = NewLocomotion; };
	FORCEINLINE void SetLifeState(ELifeState NewLifeState) { LifeState = NewLifeState; };
	FORCEINLINE void SetStand(EArcherStand NewStand) { Stand = NewStand; };

	FORCEINLINE void SetIsJumping(bool State) { IsJumping = State; };
	FORCEINLINE void SetIsFalling(bool State) { IsFalling = State; };

	void RechargeBow();
	void TakeBackBow();
	void Release();
	void DrawBow(float BowSpeed);
	void StopDraw();

protected:
	UPROPERTY(BlueprintReadWrite) float PitchValue = 0.f;
	UPROPERTY(BlueprintReadWrite) bool IsAiming = false;
	UPROPERTY(BlueprintReadWrite) bool IsDrawing = false;
	UPROPERTY(BlueprintReadWrite) bool IsBowBack = false;

	UPROPERTY(BlueprintReadWrite) bool IsJumping = false;
	UPROPERTY(BlueprintReadWrite) bool IsFalling = false;

	UPROPERTY(BlueprintReadWrite) EArcherStand Stand = EArcherStand::Standing;
	UPROPERTY(BlueprintReadWrite) ELifeState LifeState = ELifeState::Alive;
	UPROPERTY(BlueprintReadWrite) ELocomotion Locomotion = ELocomotion::Idle;

private:
	UAnimMontage* DrawMontage;
	UAnimMontage* TakeBackBowMontage;
	
	//ANIMATION FUNCTIONS
	void PlayMontage(UAnimMontage* Montage, float PlayRate = 1.f);
	void StopMontage(UAnimMontage* Montage, float Blend);
	void MontageJump(UAnimMontage* Montage, FString Section);
	bool IsMontagePlaying(UAnimMontage* Montage);
	FName GetMontageSection(UAnimMontage* Montage);
	
};
