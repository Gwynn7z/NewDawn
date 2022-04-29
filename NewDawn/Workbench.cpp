#include "Workbench.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "AllyDrone.h"
#include "Engine/Engine.h"

AWorkbench::AWorkbench()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetRelativeLocation(FVector(0.f, 0.f, 165.f));
	CollisionBox->SetRelativeScale3D(FVector(8.f, 8.f, 8.f));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWorkbench::Overlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AWorkbench::EndOverlap);
}

void AWorkbench::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWorkbench::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWorkbench::Overlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AAllyDrone::StaticClass())) {
		AAllyDrone* Drone = Cast<AAllyDrone>(OtherActor);
		if (Drone) Drone->SetOverlappingWorkbench(true);
	}
}

void AWorkbench::EndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AAllyDrone::StaticClass())) {
		AAllyDrone* Drone = Cast<AAllyDrone>(OtherActor);
		if(Drone) Drone->SetOverlappingWorkbench(false);
	}
}