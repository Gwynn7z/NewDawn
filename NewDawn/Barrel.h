#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Structs.h"
#include "Barrel.generated.h"

class USoundBase;

UCLASS()
class NEWDAWN_API ABarrel : public AActor
{
	GENERATED_BODY()
	
public:
	ABarrel();

	UPROPERTY(EditAnywhere, Category = "Componenti") UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Componenti") UStaticMesh* ExplodedMesh;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystemComponent* Explosion;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystemComponent* Fire;

	UFUNCTION() void Explode();
	FORCEINLINE UFUNCTION() bool IsExploded() { return AlreadyExploded; };

private:
	FAggressiveStats ExplosionDamage;
	bool AlreadyExploded;
	float ExplosionRange;

	USoundCue* ExplosionSound;

	const FVector ExplosioneRelativeLocation = FVector(0.f, 0.f, 80.f);
	const FVector FireRelativeLocation = FVector(0.f, 0.f, 90.f);

};
