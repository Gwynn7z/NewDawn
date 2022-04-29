#include "Arrow.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "TimerManager.h"
#include "Target.h"
#include "Structs.h"
#include "Bow.h"
#include "Archer.h"
#include "Enemy.h"
#include "Barrel.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AArrow::AArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SkelMesh(TEXT("StaticMesh'/Game/Arrow/Mesh/Arrow.Arrow'"));
	Mesh->SetStaticMesh(SkelMesh.Object);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetNotifyRigidBodyCollision(true);

	SetActorEnableCollision(false);

	static ConstructorHelpers::FObjectFinder<USoundCue> SoundImpactCue(TEXT("SoundCue'/Game/Arrow/Sound/ArrowImpactCue.ArrowImpactCue'"));
	ImpactSound = SoundImpactCue.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ArrowTrailAsset(TEXT("NiagaraSystem'/Game/Particles/Trail/NiagaraSystemTrail.NiagaraSystemTrail'"));
	ArrowTrail = ArrowTrailAsset.Object;

	Speed = 0.f;
	isFlying = false;
	Direzione = FVector::ZeroVector;
}

void AArrow::BeginPlay()
{
	Super::BeginPlay();

	HeadMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(Mesh->GetMaterialIndex(FName("Head"))), this);
	HeadMaterial->SetVectorParameterValue(FName("EmitterColor"), FLinearColor::Black);
	Mesh->SetMaterial(Mesh->GetMaterialIndex(FName("Head")), HeadMaterial);
}

void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isFlying) {
		FVector ActorLocation = GetActorLocation();
		FVector Increaser = GetActorForwardVector() * Speed;

		AddActorWorldOffset(Increaser * DeltaTime, true);

		Increaser.Z -= DeltaTime * 981.f;
		FVector NextLocation = ActorLocation + Increaser;
		SetActorRotation((NextLocation - ActorLocation).Rotation());
	}
}

void AArrow::SetArrowType(EDamageTypes Type)
{
	ArrowType = Type;
	switch (Type)
	{
	case EDamageTypes::Physical:
		HeadMaterial->SetVectorParameterValue(FName("EmitterColor"), FLinearColor::Black);
		break;
	case EDamageTypes::Tear:
		HeadMaterial->SetVectorParameterValue(FName("EmitterColor"), FLinearColor::White);
		break;
	case EDamageTypes::Fire:
		HeadMaterial->SetVectorParameterValue(FName("EmitterColor"), FLinearColor::Red);
		break;
	case EDamageTypes::Ice:
		HeadMaterial->SetVectorParameterValue(FName("EmitterColor"), FLinearColor::Blue);
		break;
	case EDamageTypes::Electric:
		HeadMaterial->SetVectorParameterValue(FName("EmitterColor"), FLinearColor::Yellow);
		break;
	default:
		break;
	}
	Mesh->SetMaterial(Mesh->GetMaterialIndex(FName("Head")), HeadMaterial);
}

void AArrow::SetDamage(TMap<EDamageTypes, float> Values, float BowStrength, float Increaser)
{
	Values[EDamageTypes::Physical] = (Values[EDamageTypes::Physical] * Increaser) / 100.f; //% increaser of the damage
	Values[EDamageTypes::Tear] = (Values[EDamageTypes::Tear] * Increaser) / 100.f; //% increaser of the damage
	Speed = Increaser * BowStrength;
	
	TMap<EDamageTypes, float> FinalDamages = GetVoidDamageMap();

	switch(ArrowType){
	case EDamageTypes::Physical:
		FinalDamages[ArrowType] = Values[ArrowType];
		FinalDamages[EDamageTypes::Tear] = Values[EDamageTypes::Tear] / 2;
		break;
	case EDamageTypes::Tear:
	case EDamageTypes::Fire:
	case EDamageTypes::Ice:
	case EDamageTypes::Electric:
		FinalDamages[ArrowType] = Values[ArrowType];
		FinalDamages[EDamageTypes::Physical] = Values[EDamageTypes::Physical] / 2;
		break;
	}

	ArrowStatistics = FAggressiveStats(FinalDamages);
}

void AArrow::SetReleaseInfo(FVector Location, FRotator Rotation) 
{
	SetActorLocation(Location);
	SetActorRotation(Rotation);
}

void AArrow::Release()
{
	isFlying = true;
	GetWorld()->GetTimerManager().SetTimer(DestroyArrowTimer, this, &AArrow::DestroyArrow, 5.f, false);
	SetActorEnableCollision(true);
	CurrentTrail = UNiagaraFunctionLibrary::SpawnSystemAttached(ArrowTrail, Mesh, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
}

void AArrow::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	bool bShouldIgnoreActor = false;
	if (Other->IsA(ABow::StaticClass())) bShouldIgnoreActor = true;
	else if (Other->IsA(AArcher::StaticClass())) bShouldIgnoreActor = true;
	else if (Other->IsA(AArrow::StaticClass())) bShouldIgnoreActor = true;

	if (bShouldIgnoreActor == true) {
		Mesh->IgnoreActorWhenMoving(Other, true);
		return;
	}

	if (isFlying) {

		FVector Increaser = GetActorForwardVector() * 25.f;
		AddActorWorldOffset(Increaser); //Penetration

		isFlying = false;
		if (CurrentTrail) CurrentTrail->Deactivate();
		GetWorld()->GetTimerManager().ClearTimer(DestroyArrowTimer);
		AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, HitLocation);
		SetActorEnableCollision(false);
	}

	if (Other->IsA(AEnemy::StaticClass())) {
		AEnemy* Enemy = Cast<AEnemy>(Other);
		if (Enemy->GetState() != EEnemyState::Broken) Enemy->GetHit(ArrowStatistics.Stats, Hit);
	}
	else if (Other->IsA(ABarrel::StaticClass())) {
		ABarrel* Barrel = Cast<ABarrel>(Other);
		if (Barrel) {
			if (!Barrel->IsExploded()) Destroy();
			Barrel->Explode();
		}
	}
	else if (Other->IsA(ATarget::StaticClass()) && Hit.BoneName.IsEqual(FName("target"))) DestroyArrow();
}

void AArrow::DestroyArrow()
{
	Destroy();
}
