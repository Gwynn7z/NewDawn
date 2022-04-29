#include "Weight.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AWeight::AWeight() {
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeightComponent"));
	SetRootComponent(Mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshAsset(TEXT("StaticMesh'/Game/Items/Weight/Weight.Weight'"));
	Mesh->SetStaticMesh(BaseMeshAsset.Object);
	Mesh->SetSimulatePhysics(true);
}