#include "Barrel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "AllyDrone.h"
#include "Enemy.h"
#include "Archer.h"
#include "Kismet/GameplayStatics.h"

ABarrel::ABarrel() {
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshAsset(TEXT("StaticMesh'/Game/Items/Barrels/NitrogenTank.NitrogenTank'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ExplodedBaseMeshAsset(TEXT("StaticMesh'/Game/Items/Barrels/NitrogenTank.NitrogenTank'"));
	Mesh->SetStaticMesh(BaseMeshAsset.Object);
	ExplodedMesh = ExplodedBaseMeshAsset.Object;

	Explosion = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionParticle"));
	Explosion->SetupAttachment(RootComponent);
	Explosion->SetRelativeLocation(ExplosioneRelativeLocation);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAssetExplosion(TEXT("ParticleSystem'/Game/Particles/AdvancedMagicFX13/Particles/P_ky_explosion3.P_ky_explosion3'"));
	if (ParticleAssetExplosion.Succeeded()) Explosion->SetTemplate(ParticleAssetExplosion.Object);
	Explosion->bAutoActivate = false;

	Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));
	Fire->SetupAttachment(RootComponent);
	Fire->SetRelativeLocation(FireRelativeLocation);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAssetFire(TEXT("ParticleSystem'/Game/Particles/StarterContent/Particles/P_Fire.P_Fire'"));
	if (ParticleAssetFire.Succeeded()) Fire->SetTemplate(ParticleAssetFire.Object);
	Fire->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<USoundCue> ExplosionSoundCue(TEXT("SoundCue'/Game/Turret/Sound/Shoot_Cue.Shoot_Cue'"));
	ExplosionSound = ExplosionSoundCue.Object;
	
	AlreadyExploded = false;
	ExplosionDamage = FAggressiveStats(GetDamageMapFromValues(100.f, 0.f, 50.f, 0.f, 0.f));

	Mesh->SetSimulatePhysics(true);
	ExplosionRange = 500.f;
}

void ABarrel::Explode()
{
	if (!AlreadyExploded) {
		AlreadyExploded = true;

		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
		Explosion->Activate();
		Fire->Activate();
		Mesh->SetStaticMesh(ExplodedMesh);

		FCollisionShape CollSphere = FCollisionShape::MakeSphere(ExplosionRange);

		TArray<FHitResult> Hits;
		GetWorld()->SweepMultiByChannel(Hits, GetActorLocation(), Mesh->GetComponentLocation() + FVector(1.f, 1.f, 1.f), FRotator::ZeroRotator.Quaternion(), ECollisionChannel::ECC_Visibility, CollSphere);

		for (FHitResult& Hit : Hits) {
			if (Hit.bBlockingHit) {
				AActor* OtherActor = Hit.GetActor();
				if (OtherActor->IsA(AEnemy::StaticClass())) {
					AEnemy* Enemy = Cast<AEnemy>(OtherActor);
					if (Enemy) Enemy->GetHit(ExplosionDamage.Stats);
				}
				else if (OtherActor->IsA(AArcher::StaticClass())) {
					AArcher* Archer = Cast<AArcher>(OtherActor);
					if (Archer) Archer->GetHit(ExplosionDamage.Stats);
				}
				else if (OtherActor->IsA(AAllyDrone::StaticClass())) {
					AAllyDrone* Drone = Cast<AAllyDrone>(OtherActor);
					if (Drone) Drone->GetHit(ExplosionDamage.Stats);
				}
				else if (OtherActor->IsA(ABarrel::StaticClass())) {
					ABarrel* Barrel = Cast<ABarrel>(OtherActor);
					if (Barrel) Barrel->Explode();
				}
			}
		}
	}
}

