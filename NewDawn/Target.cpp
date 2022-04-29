#include "Target.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Projectile.h"
#include "Arrow.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "TargetAnimInstance.h"
#include "TimerManager.h"

ATarget::ATarget()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh->DestroyComponent();

	SkelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkelMesh"));
	SetRootComponent(SkelMesh);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("SkeletalMesh'/Game/Triggers/Target/Mesh/Target.Target'"));
	SkelMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	SkelMesh->SetSkeletalMesh(MeshAsset.Object);
	SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkelMesh->SetCollisionProfileName(FName("BlockAllDynamics"));
	SkelMesh->OnComponentHit.AddDynamic(this, &ATarget::OnHit);

	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBlueprint(TEXT("AnimBlueprint'/Game/Triggers/Target/Animations/AnimBP.AnimBP'"));
	if (AnimBlueprint.Object != NULL)
	{
		UAnimBlueprint* AnimBP = AnimBlueprint.Object;
		SkelMesh->SetAnimInstanceClass(AnimBP->GetAnimBlueprintGeneratedClass());
	}

	TargetType = ETargetType::Arrow;
}

void ATarget::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UTargetAnimInstance>(SkelMesh->GetAnimInstance());

	TargetMaterial = UMaterialInstanceDynamic::Create(SkelMesh->GetMaterial(SkelMesh->GetMaterialIndex(FName("M_Target"))), this);
	FLinearColor Color = FLinearColor::Yellow;
	if (TargetType == ETargetType::Projectile) Color = FLinearColor::Red;

	TargetMaterial->SetVectorParameterValue(FName("Color"), Color);
	SkelMesh->SetMaterial(SkelMesh->GetMaterialIndex(FName("M_Target")), TargetMaterial);

	GetWorld()->GetTimerManager().SetTimer(OpenTimer, this, &ATarget::Open, 4.f, false);
}

void ATarget::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "ciao");
	if (Hit.BoneName.IsEqual(FName("target"))) {
		if (OtherActor->IsA(AProjectile::StaticClass()) && TargetType == ETargetType::Projectile) ToggleSlider();
		else if (OtherActor->IsA(AArrow::StaticClass()) && TargetType == ETargetType::Arrow) ToggleSlider();
	}
}

void ATarget::Open()
{
	if (isOpen) {
		GetWorld()->GetTimerManager().SetTimer(OpenTimer, this, &ATarget::Open, 4.f, false);
		isOpen = false;
		AnimInstance->SetIsOpen(false);
	}
	else {
		GetWorld()->GetTimerManager().SetTimer(OpenTimer, this, &ATarget::Open, 2.5f, false);
		isOpen = true;
		AnimInstance->SetIsOpen(true);
	}
}