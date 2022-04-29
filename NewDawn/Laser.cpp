#include "Laser.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#include "Enemy.h"
#include "Archer.h"
#include "AllyDrone.h"

ALaser::ALaser()
{
	PrimaryActorTick.bCanEverTick = false;

	RootOverride = CreateDefaultSubobject<USceneComponent>(TEXT("RootOverride"));
	SetRootComponent(RootOverride);

	LaserParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserParticle"));
	LaserParticle->SetupAttachment(RootComponent);
	LaserParticle->SetRelativeScale3D(LaserScale);
	LaserParticle->SetRelativeLocation(LaserLocation);
	LaserParticle->SetRelativeRotation(LaserRotation);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAssetLaser(TEXT("ParticleSystem'/Game/Particles/ScienceLab/PS_electricity_arcA.PS_electricity_arcA'"));
	if (ParticleAssetLaser.Succeeded()) LaserParticle->SetTemplate(ParticleAssetLaser.Object);
	LaserParticle->bAutoActivate = true;

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageZone"));
	Hitbox->SetupAttachment(RootComponent);
	Hitbox->SetRelativeScale3D(HitboxScale);
	Hitbox->SetRelativeLocation(HiboxLocation);
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &ALaser::Hit);

	LaserDamage = FAggressiveStats(GetDamageMapFromValues(9999.f, 0.f, 0.f, 0.f, 9999.f));
}

void ALaser::Hit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AEnemy::StaticClass())) {
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy) Enemy->GetHit(LaserDamage.Stats);
	}
	else if (OtherActor->IsA(AArcher::StaticClass())) {
		AArcher* Archer = Cast<AArcher>(OtherActor);
		if (Archer) Archer->GetHit(LaserDamage.Stats);
	}
	else if (OtherActor->IsA(AAllyDrone::StaticClass())) {
		AAllyDrone* Drone = Cast<AAllyDrone>(OtherActor);
		if (Drone) Drone->GetHit(LaserDamage.Stats);
	}
}