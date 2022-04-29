#include "Battery.h"
#include "Enums.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

ABattery::ABattery()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(Mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshAsset(TEXT("StaticMesh'/Game/Items/Battery/Battery.Battery'"));
	Mesh->SetStaticMesh(BaseMeshAsset.Object);
	Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	Spark = CreateDefaultSubobject<UParticleSystem>(TEXT("Spark"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/Particles/FXVarietyPack/Particles/P_ky_hit1.P_ky_hit1'"));
	if (ParticleAsset.Succeeded()) Spark = ParticleAsset.Object;

	Charge = 0;
}

void ABattery::ParticleEffect()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Spark, GetActorLocation());
}