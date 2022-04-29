#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Structs.h"
#include "Projectile.generated.h"

UCLASS()
class NEWDAWN_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	

	void SetDamage(FAggressiveStats Damages, float NewSpeed);
	UFUNCTION() virtual void Shoot();
	UFUNCTION() void DestroyProjectile();
	UFUNCTION() void ResetTimer();


protected:
	void Hit(AActor* OtherActor, FVector HitLocation, const FHitResult& Hit);

	FAggressiveStats ProjectileDamage;

	float Speed;
	bool isFlying;

	FVector Direzione;
	FRotator Rotazione;

	FTimerHandle DestroyProjectileTimer;
};
