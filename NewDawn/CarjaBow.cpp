#include "CarjaBow.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ACarjaBow::ACarjaBow()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkelMesh(TEXT("SkeletalMesh'/Game/Bows/CarjaBow/Mesh/CarjaBow.CarjaBow'"));
	Mesh->SetSkeletalMesh(SkelMesh.Object);
	Mesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));

	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBlueprint(TEXT("AnimBlueprint'/Game/Bows/CarjaBow/Animations/CarjaBowAnimBP.CarjaBowAnimBP'"));
	if (AnimBlueprint.Object != NULL)
	{
		UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprint.Object);
		Mesh->SetAnimInstanceClass(AnimBP->GetAnimBlueprintGeneratedClass());
	}

	float PhysicalDamage = 30.f;
	float TearDamage = 20.f;
	float FireDamage = 15.f;
	TMap<EDamageTypes, float> Stats = GetDamageMapFromValues(PhysicalDamage, TearDamage, FireDamage, 0.f, 0.f);
	
	float BowStrength = 50.f;
	float BowSpeed = 125.f;
	float BowBraceHeight = -826.f;
	BowInfos = FBowStats(EBowTypes::Carja, Stats, BowStrength, BowSpeed, BowBraceHeight);

	RootBoneName = "RootBow";
}