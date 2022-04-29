#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs.h"
#include "Enums.h"
#include "Laser.generated.h"

class USceneComponent;

UCLASS()
class NEWDAWN_API ALaser : public AActor
{
	GENERATED_BODY()
	
public:
	ALaser();
	UPROPERTY(EditAnywhere, Category = "Componenti") USceneComponent* RootOverride;
	UPROPERTY(EditAnywhere, Category = "Componenti") UShapeComponent* Hitbox;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystemComponent* LaserParticle;

private:
	UFUNCTION() void Hit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	FAggressiveStats LaserDamage;

	const FVector HitboxScale = FVector(6.75f, 0.5f, 0.5f);
	const FVector HiboxLocation = FVector(220.f, 0.f, 0.f);

	const FVector LaserLocation = FVector(220.f, 0.f, 0.f);
	const FVector LaserScale = FVector(3.25f, 1.1f, 1.25f);
	const FRotator LaserRotation = FRotator(0.f, -90.f, 0.f);
};

