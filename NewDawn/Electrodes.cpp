#include "Electrodes.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "Archer.h"
#include "Barrel.h"
#include "AllyDrone.h"
#include "TimerManager.h"

AElectrodes::AElectrodes()
{
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	SetRootComponent(HitBox);
	HitBox->SetBoxExtent(FVector(5.f, 100.f, 5.f));
	HitBox->SetGenerateOverlapEvents(false);

	RightElectrode = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightMesh"));
	RightElectrode->SetupAttachment(RootComponent);
	RightElectrode->SetRelativeLocation(FVector(0.f, 20.f, 0.f));
	RightElectrode->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RightElectrode->SetCollisionProfileName("Pawn");
	RightElectrode->SetNotifyRigidBodyCollision(true);

	LeftElectrode = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftMesh"));
	LeftElectrode->SetupAttachment(RootComponent);
	LeftElectrode->SetRelativeLocation(FVector(0.f, -20.f, 0.f));
	LeftElectrode->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LeftElectrode->SetCollisionProfileName("Pawn");
	LeftElectrode->SetNotifyRigidBodyCollision(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("StaticMesh'/Game/TwinStrike/TwinStrike.TwinStrike'"));
	if(SphereAsset.Succeeded()){
		RightElectrode->SetStaticMesh(SphereAsset.Object);
		LeftElectrode->SetStaticMesh(SphereAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/Particles/ScienceLab/PS_electricity_arcA.PS_electricity_arcA'"));
	if(ParticleAsset.Succeeded()) Laser = ParticleAsset.Object;

	IsMoving = false;
	Activated = false;
	ArrivedAtFull = false;
	Speed = 200.f;
	Damage = FAggressiveStats();
}

void AElectrodes::BeginPlay()
{
	Super::BeginPlay();
}

void AElectrodes::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsMoving){
		AddActorWorldOffset(GetActorForwardVector() * DeltaTime * Speed, true);

		FVector SphereLocation = LeftElectrode->GetRelativeLocation();
		FVector SphereScale = LeftElectrode->GetRelativeScale3D();

		FVector NewSphereLocation = FMath::VInterpTo(SphereLocation, FVector(0.f, -120.f, 0.f), DeltaTime, 2.f);
		FVector NewSphereScale = FMath::VInterpTo(SphereScale, FVector(6.f, 6.f, 6.f), DeltaTime, 2.f);
		
		LeftElectrode->SetRelativeLocation(NewSphereLocation, true);
		RightElectrode->SetRelativeLocation(NewSphereLocation * -1, true);

		LeftElectrode->SetRelativeScale3D(NewSphereScale);
		RightElectrode->SetRelativeScale3D(NewSphereScale);

		if(LeftElectrode->GetRelativeLocation().Equals(FVector(0.f, -120.f, 0.f), 10.f) && LeftElectrode->GetRelativeScale3D().Equals(FVector(6.f, 6.f, 6.f), 10.f)) ArrivedAtFull = true;

		if(Activated && ArrivedAtFull){
			 IsMoving = false;
			 GetWorld()->GetTimerManager().SetTimer(LifeTimer, this, &AElectrodes::DestroyElectrodes, 20.f, false);
			 Discharge();
		}
	}
}

void AElectrodes::DestroyElectrodes()
{
	Destroy();
}

void AElectrodes::HitArea()
{
	HitBox->UpdateOverlaps();
	TArray<AActor*> OverlappingActors;
	HitBox->GetOverlappingActors(OverlappingActors);
	for(auto &OtherActor : OverlappingActors){
		if (OtherActor->IsA(AEnemy::StaticClass())) {
			AEnemy* Enemy = Cast<AEnemy>(OtherActor);
			if (Enemy != nullptr) if (Enemy->GetState() != EEnemyState::Broken) Enemy->GetHit(Damage.Stats);
		}
		else if (OtherActor->IsA(AArcher::StaticClass())) {
			AArcher* Archer = Cast <AArcher>(OtherActor);
			if (Archer != nullptr) if (Archer->GetLifeState() == ELifeState::Alive) Archer->GetHit(Damage.Stats);
		}
		else if (OtherActor->IsA(AAllyDrone::StaticClass())) {
			AAllyDrone* Drone = Cast <AAllyDrone>(OtherActor);
			if (Drone != nullptr) if (Drone->GetLifeState() != EAllyDroneState::Broken) Drone->GetHit(Damage.Stats);
		}
		else if (OtherActor->IsA(ABarrel::StaticClass())) {
			ABarrel* Barrel = Cast<ABarrel>(OtherActor);
			if (Barrel != nullptr) Barrel->Explode();
		}
	}
}

void AElectrodes::Launch(FAggressiveStats Damages)
{
	IsMoving = true;
	Damage = Damages;
}

void AElectrodes::Activate()
{
	Activated = true;
}

void AElectrodes::Discharge()
{
	HitBox->SetGenerateOverlapEvents(true);
	HitBox->UpdateOverlaps();

	UGameplayStatics::SpawnEmitterAttached(Laser, RootComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, FVector(0.5f, 0.5f, 0.5f), EAttachLocation::SnapToTarget);

	GetWorld()->GetTimerManager().SetTimer(DamageTimer, this, &AElectrodes::HitArea, 1.f, true);
}

void AElectrodes::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if(Other == GetInstigator() || Other->IsA(AElectrodes::StaticClass())){
		RightElectrode->IgnoreActorWhenMoving(Other, true);
		LeftElectrode->IgnoreActorWhenMoving(Other, true);
	}
	else{
		Speed = 0.f;
		Activate();
	}
}