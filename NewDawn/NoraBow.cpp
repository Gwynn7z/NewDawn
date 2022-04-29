#include "NoraBow.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ANoraBow::ANoraBow() 
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

	float PhysicalDamage = 60.f;
	float TearDamage = 40.f;
	TMap<EDamageTypes, float> Damage = GetDamageMapFromValues(PhysicalDamage, TearDamage, 0.f, 0.f, 0.f);

	float BowStrength = 100.f;
	float BowSpeed = 75.f;
	float BowBraceHeight = 66.f;
	BowInfos = FBowStats(EBowTypes::Nora, Damage, BowStrength, BowSpeed, BowBraceHeight);

	RootBoneName = "root";
}