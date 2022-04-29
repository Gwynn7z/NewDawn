#include "Shot.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AShot::AShot()
{
    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
    SetRootComponent(Box);
    Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Box->SetCollisionProfileName(FName("Pawn"));
	Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Box->SetNotifyRigidBodyCollision(true);
    Box->SetBoxExtent(FVector(50.f, 5.f ,5.f)); 
    

    static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/Particles/ScienceLab/PS_electricity_arcA.PS_electricity_arcA'"));
    Shot = ParticleAsset.Object;
}

void AShot::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
    if(Other == GetInstigator()) Box->IgnoreActorWhenMoving(Other, true);
    else this->Hit(Other, HitLocation, Hit);
}

void AShot::Shoot()
{
    UGameplayStatics::SpawnEmitterAttached(Shot, Box, NAME_None, FVector::ZeroVector, FRotator(0.f, 90.f, 0.f), ShotScale, EAttachLocation::SnapToTarget);
    Super::Shoot();
}