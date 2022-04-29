#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Structs.h"
#include "Arrow.generated.h"

class USoundCue;
class UParticleSystem;
class UNiagaraSystem;
class UNiagaraComponent;
class UMaterialInstanceDynamic;

UCLASS()
class NEWDAWN_API AArrow : public AActor
{
	GENERATED_BODY()

public:
	AArrow();
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, Category = "Componenti") UStaticMeshComponent* Mesh;

	void SetDamage(TMap<EDamageTypes, float> Values, float BowStrength, float Increaser);
	UFUNCTION() void SetReleaseInfo(FVector Location, FRotator Rotation);
	UFUNCTION() void Release();
	UFUNCTION() void DestroyArrow();
	UFUNCTION() void SetArrowType(EDamageTypes Type);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Componenti") UNiagaraSystem* ArrowTrail;
	UNiagaraComponent* CurrentTrail;
	UMaterialInstanceDynamic* HeadMaterial;
	FAggressiveStats ArrowStatistics;
	EDamageTypes ArrowType;

	float Speed;
	bool isFlying;

	FVector Direzione;
	FRotator Rotazione;

	FTimerHandle DestroyArrowTimer;
	USoundCue* ImpactSound;
};
