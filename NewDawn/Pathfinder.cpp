#include "Pathfinder.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

APathfinder::APathfinder()
{
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);
	Box->SetRelativeScale3D(FVector(50.f,50.f, 50.f));

	Dimensions = FVector(200.f, 200.f, 200.f);
}

void APathfinder::BeginPlay()
{
	Super::BeginPlay();

	FVector BoxOrigin = GetActorLocation() - Box->Bounds.BoxExtent;
	FVector BoxDimensions = Box->Bounds.BoxExtent * 2;
	
	FVector NextLocation = BoxOrigin;
	for (int32 i = 0; i < BoxDimensions.Z / Dimensions.Z; i++) {
		if(i != 0) NextLocation.Z += Dimensions.Z;
		NextLocation.Y = BoxOrigin.Y;
		for (int32 j = 0; j < BoxDimensions.Y / Dimensions.Y; j++) {
			if(j != 0) NextLocation.Y += Dimensions.Y;
			 NextLocation.X = BoxOrigin.X;
			for (int32 k = 0; k < BoxDimensions.X / Dimensions.X; k++) {
				if(k != 0) NextLocation.X += Dimensions.X;
				Location.Add(NextLocation + Dimensions / 2);		
			}
		}
	}
}

void APathfinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	for (int32 i = 0; i < Location.Num(); i++) {

		FHitResult Hit;
		FCollisionShape CollBox = FCollisionShape::MakeBox(Dimensions/2);

		DrawDebugBox(GetWorld(), Location[i], Dimensions/2, FColor::Red);

		bool isHit = GetWorld()->SweepSingleByChannel(Hit, Location[i], Location[i] + FVector(0.f, 0.f, 1.f), FQuat(FRotator::ZeroRotator), ECC_Visibility, CollBox);
		if (isHit) {
			Location.RemoveAt(i);
		}
	}
}

