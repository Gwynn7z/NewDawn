#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Bullet.generated.h"

UCLASS()
class NEWDAWN_API ABullet : public AProjectile
{
	GENERATED_BODY()

	public:
		ABullet();
		UPROPERTY(EditAnywhere, Category = "Componenti") UStaticMeshComponent* Mesh;

	protected:
		virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};
