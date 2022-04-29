#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ABullet::ABullet()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> NormalMesh(TEXT("StaticMesh'/Game/Projectile/Projectile.Projectile'"));

	Mesh->SetStaticMesh(NormalMesh.Object);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetNotifyRigidBodyCollision(true);
}

void ABullet::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Other == GetOwner()) Mesh->IgnoreActorWhenMoving(Other, true);
	else this->Hit(Other, HitLocation, Hit);
}