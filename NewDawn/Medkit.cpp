#include "Medkit.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Items/MedkitInventoryItem.h"
#include "Items/UsableInventoryItem.h"

AMedkit::AMedkit()
{
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshAsset(TEXT("StaticMesh'/Game/Items/Medkit/Medkit.Medkit'"));
	Mesh->SetStaticMesh(BaseMeshAsset.Object);
	Mesh->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));

	ItemValues = MakeTuple<EUsableTypes, int32>(EUsableTypes::Medkit, 1);
}