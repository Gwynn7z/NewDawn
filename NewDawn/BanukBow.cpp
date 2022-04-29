#include "BanukBow.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ABanukBow::ABanukBow()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkelMesh(TEXT("SkeletalMesh'/Game/Bows/NoraBow/Mesh/NoraBow.NoraBow'"));
	Mesh->SetSkeletalMesh(SkelMesh.Object);
	Mesh->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));
	
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBlueprint(TEXT("AnimBlueprint'/Game/Bows/NoraBow/Animations/NoraBowAnimBP.NoraBowAnimBP'"));
	if (AnimBlueprint.Object != NULL)
	{
		UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprint.Object);
		Mesh->SetAnimInstanceClass(AnimBP->GetAnimBlueprintGeneratedClass());
	}

	float FireDamage = 20.f;
	float IceDamage = 20.f;
	float ElectricDamage = 20.f;
	TMap<EDamageTypes, float> Stats = GetDamageMapFromValues(0.f, 0.f, FireDamage, IceDamage, ElectricDamage);

	float BowStrength = 50.f;
	float BowSpeed = 100.f;
	float BowBraceHeight = 66.f;
	BowInfos = FBowStats(EBowTypes::Banuk, Stats, BowStrength, BowSpeed, BowBraceHeight);

	RootBoneName = "root";
}