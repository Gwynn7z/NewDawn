#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "Enums.h"
#include "Target.generated.h"

class UMaterialInstanceDynamic;
class UTargetAnimInstance;

UCLASS()
class NEWDAWN_API ATarget : public ATrigger
{
	GENERATED_BODY()

public:
	ATarget();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Componenti") USkeletalMeshComponent* SkelMesh;
	UPROPERTY(EditAnywhere, Category = "Variabili") ETargetType TargetType;

private:
	UMaterialInstanceDynamic* TargetMaterial;
	UTargetAnimInstance* AnimInstance;
	bool isOpen = false;
	FTimerHandle OpenTimer;

	UFUNCTION() void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION() void Open();
};


