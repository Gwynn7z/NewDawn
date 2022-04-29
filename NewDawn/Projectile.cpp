#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Enemy.h"
#include "Archer.h"
#include "AllyDrone.h"
#include "Barrel.h"
#include "TimerManager.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(true);

	ProjectileDamage = FAggressiveStats();
	Speed = 0.f;
	isFlying = false;
	Direzione = FVector::ZeroVector;
}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isFlying) {
		FVector Direction = GetActorForwardVector() * DeltaTime * Speed;
		AddActorWorldOffset(Direction, true);
	}
}

void AProjectile::Hit(AActor* OtherActor, FVector HitLocation, const FHitResult& Hit)
{
	if (isFlying) {	 
		GetWorld()->GetTimerManager().ClearTimer(DestroyProjectileTimer);

		if (OtherActor->IsA(AEnemy::StaticClass())) {
			AEnemy* Enemy = Cast<AEnemy>(OtherActor);
			if (Enemy != nullptr) if (Enemy->GetState() != EEnemyState::Broken && Enemy != GetOwner()) Enemy->GetHit(ProjectileDamage.Stats, Hit);
		}
		else if (OtherActor->IsA(AArcher::StaticClass())) {
			AArcher* Archer = Cast <AArcher>(OtherActor);
			if (Archer != nullptr) if (Archer->GetLifeState() == ELifeState::Alive) Archer->GetHit(ProjectileDamage.Stats);
		}
		else if (OtherActor->IsA(AAllyDrone::StaticClass())) {
			AAllyDrone* Drone = Cast <AAllyDrone>(OtherActor);
			if (Drone != nullptr) if (Drone->GetLifeState() != EAllyDroneState::Broken) Drone->GetHit(ProjectileDamage.Stats);
		}
		else if (OtherActor->IsA(ABarrel::StaticClass())) {
			ABarrel* Barrel = Cast<ABarrel>(OtherActor);
			if (Barrel != nullptr) Barrel->Explode();
		}
		DestroyProjectile();
	}
}

void AProjectile::SetDamage(FAggressiveStats Damages, float NewSpeed)
{
	ProjectileDamage = Damages;
	Speed = NewSpeed * 100.f;
}

void AProjectile::Shoot()
{
	isFlying = true;
	GetWorld()->GetTimerManager().SetTimer(DestroyProjectileTimer, this, &AProjectile::DestroyProjectile, 3.f, false);
}

void AProjectile::DestroyProjectile()
{
	Destroy();
}

void AProjectile::ResetTimer()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(DestroyProjectileTimer)) {
		GetWorld()->GetTimerManager().ClearTimer(DestroyProjectileTimer);
		GetWorld()->GetTimerManager().SetTimer(DestroyProjectileTimer, this, &AProjectile::DestroyProjectile, 3.f, false);
	}
}


