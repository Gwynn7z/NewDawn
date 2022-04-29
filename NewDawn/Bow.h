#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Structs.h"
#include "Bow.generated.h"

class UAnimBlueprint;
class UAnimMontage;
class AArrow;
class USoundCue;
class USkeletalMesh;
class UBowAnimInstance;
class UBowInventoryItem;

UCLASS()
class NEWDAWN_API ABow : public AActor
{
	GENERATED_BODY()

public:
	ABow();
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Components")    USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations") UBowAnimInstance* AnimInstance;

	UFUNCTION() void Draw();
	UFUNCTION() void Release(FVector Location, FRotator Rotation);

	UFUNCTION() void Recharge();
	UFUNCTION() void ResetDraw();

	UFUNCTION() void RemoveArrow();
	UFUNCTION() void SetCurrentArrowType(EDamageTypes ArrowType);

	FORCEINLINE UFUNCTION() bool HasArrow() { return  Arrow != nullptr; };
	FORCEINLINE UFUNCTION() float GetDrawValue() { return DrawValue; };

	FORCEINLINE UFUNCTION() void SetStringLocation(FVector Location) { StringLocation = Location; };

	FORCEINLINE FBowStats GetBowStats() { return BowInfos; };

protected:
	virtual void BeginPlay() override;
	FBowStats BowInfos;
	FString RootBoneName;

private:
	void SetDrawValue(float Value);
	void SetIsDrawing(bool Drawing);

	FVector StringLocation;

	float DrawValue;
	bool IsDrawing;
	bool IsStringBending;

	AArrow* Arrow;
	EDamageTypes CurrentArrowType;

	UAudioComponent* StringPullAudioComponent;
	USoundCue* StringPullSound;
	USoundCue* StringReleaseSound;
	USoundCue* KnockArrowSound;
};
