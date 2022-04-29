#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Shot.generated.h"

class UBoxComponent;

UCLASS()
class NEWDAWN_API AShot : public AProjectile
{
	GENERATED_BODY()

	public:
		AShot();

		UPROPERTY(EditAnywhere, Category="Componenti") UBoxComponent* Box;
		UPROPERTY(EditAnywhere, Category="Componenti") UParticleSystem* Shot;

		void Shoot() override;

	private:
		FVector ShotScale = FVector(0.5f, 0.5f, 0.5f);
	protected:
			virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};