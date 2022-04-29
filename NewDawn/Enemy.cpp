#include "Enemy.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "TimerManager.h"
#include "Archer.h"
#include "EnemyAI.h"
#include "Items/Inventory.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Radar = CreateDefaultSubobject<USpotLightComponent>(TEXT("Radar"));

	BodyHitParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("BodyHitParticle"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BodyHitParticleAsset(TEXT("ParticleSystem'/Game/Particles/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_GolemBot/Notifies/P_SparkBurst.P_SparkBurst'"));
	if (BodyHitParticleAsset.Succeeded()) BodyHitParticle = BodyHitParticleAsset.Object;

	BoneHitParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("BoneHitParticle"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BoneHitParticleAsset(TEXT("ParticleSystem'/Game/Particles/InfinityBladeEffects/Effects/FX_Combat_Base/Impact/P_ImpactSpark.P_ImpactSpark'"));
	if (BoneHitParticleAsset.Succeeded()) BoneHitParticle = BoneHitParticleAsset.Object;

	Fire = CreateDefaultSubobject<UParticleSystem>(TEXT("Fire"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FireParticleAsset(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
	if (FireParticleAsset.Succeeded()) Fire = FireParticleAsset.Object;

	Ice = CreateDefaultSubobject<UParticleSystem>(TEXT("Ice"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> IceParticleAsset(TEXT("ParticleSystem'/Game/Particles/InfinityBladeEffects/Effects/FX_Mobile/ICE/P_IceWall_SpawnLoop.P_IceWall_SpawnLoop'"));
	if (IceParticleAsset.Succeeded()) Ice = IceParticleAsset.Object;

	Electric = CreateDefaultSubobject<UParticleSystem>(TEXT("Electric"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ElectricParticleAsset(TEXT("ParticleSystem'/Game/Particles/InfinityBladeEffects/Effects/FX_Combat_Lightning/P_DOT_Lightning_01.P_DOT_Lightning_01'"));
	if (ElectricParticleAsset.Succeeded()) Electric = ElectricParticleAsset.Object;

	Sparks = CreateDefaultSubobject<UParticleSystem>(TEXT("Sparks"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> SparksParticleAsset(TEXT("ParticleSystem'/Game/Particles/StarterContent/Particles/P_Sparks.P_Sparks'"));
	if (SparksParticleAsset.Succeeded()) Sparks = SparksParticleAsset.Object;

	ShootSpark = CreateDefaultSubobject<UParticleSystem>(TEXT("ShootSpark"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ShootSparksParticleAsset(TEXT("ParticleSystem'/Game/Particles/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Spider/Fire/P_Spider_Proj_Fire_SM.P_Spider_Proj_Fire_SM'"));
	if (ShootSparksParticleAsset.Succeeded()) ShootSpark = ShootSparksParticleAsset.Object;


	ElementalState = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ElemParticle"));
	ElementalState->SetupAttachment(RootComponent);
	ElementalState->bAutoActivate = false;

	EnemyState = EEnemyState::Idle;
	ElemState = EElementalState::None;

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetNotifyRigidBodyCollision(true);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Target = nullptr;

	Loot = FUnownedInventory();
}

void AEnemy::GetHit(TMap<EDamageTypes, float> Damage, const FHitResult& Hit)
{
	switch (ElemState) {
	case EElementalState::Frozen:
		Damage[EDamageTypes::Physical] *= 2.f;
		Damage[EDamageTypes::Ice] = 0.f;
		break;
	case EElementalState::OnFire:
		Damage[EDamageTypes::Fire] = 0.f;
		break;
	case EElementalState::Stunned:
		Damage[EDamageTypes::Electric] = 0.f;
		break;
	}

	for (auto& SkelPart : SkeletalComponents) {
		if (SkelPart.ConstraintName == Hit.BoneName || Mesh->BoneIsChildOf(Hit.BoneName, SkelPart.ConstraintName)) {
			FString BoneName = Hit.BoneName.ToString();

			SkelPart.DefensiveStats.ModifyStats(Damage, EModifierType::Decrease);

			TArray<EDamageTypes> SkelPartIgnoringStats = SkelPart.DefensiveStats.GetIgnoredDamages();
			for (auto& Stat : Damage) {
				if (!SkelPartIgnoringStats.Contains(Stat.Key)) Stat.Value = 0.f;
			}
			Damage[EDamageTypes::Physical] *= SkelPart.GetDamageModifier();

			MainStats.ModifyStats(Damage, EModifierType::Decrease);

			if (SkelPart.DefensiveStats.IsZeroValue(EDamageTypes::Tear)) {
				Loot.UpdateResource(EResourceTypes::CopperFragment, 10, EModifierType::Increase);
				Loot.UpdateResource(EResourceTypes::MetalPlate, FMath::RandRange(1, 4), EModifierType::Increase);
				Loot.UpdateResource(EResourceTypes::Wire, FMath::RandRange(1, 3), EModifierType::Increase);
				Loot.UpdateResource(EResourceTypes::ElectronicComponent, FMath::RandRange(1, 2), EModifierType::Increase);
				SkelPart.BreakConstraint();
				BreakConstraint(SkelPart.ConstraintName);
				FString AttachedSockString = SkelPart.ConstraintName.ToString() + "VFXAttachedSock"; //Socket for sparks
				FString DetachedSockString = SkelPart.ConstraintName.ToString() + "VFXDetachedSock"; //Socket for sparks
				UGameplayStatics::SpawnEmitterAttached(Sparks, Mesh, *AttachedSockString, FVector::ZeroVector, FRotator::ZeroRotator,FVector(1.f, 1.f, 1.f), EAttachLocation::SnapToTarget);
				UGameplayStatics::SpawnEmitterAttached(Sparks, Mesh, *DetachedSockString, FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), EAttachLocation::SnapToTarget);
			}
			
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BoneHitParticle, Hit.Location, FRotator::ZeroRotator, FVector(0.5f, 0.5f, 0.5f));
			CheckExceedingStats();
			return;
		}
	}

	MainStats.ModifyStats(Damage, EModifierType::Decrease);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BodyHitParticle, Hit.Location, FRotator::ZeroRotator, FVector(0.5f, 0.5f, 0.5f));
	CheckExceedingStats();
}

void AEnemy::GetHit(TMap<EDamageTypes, float> Damage)
{
	switch (ElemState) {
	case EElementalState::Frozen:
		Damage[EDamageTypes::Physical] *= 2.f;
		Damage[EDamageTypes::Ice] = 0.f;
		break;
	case EElementalState::OnFire:
		Damage[EDamageTypes::Fire] = 0.f;
		break;
	case EElementalState::Stunned:
		Damage[EDamageTypes::Electric] = 0.f;
		break;
	}

	MainStats.ModifyStats(Damage, EModifierType::Decrease);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BodyHitParticle, Mesh->GetComponentLocation());
	CheckExceedingStats();
}

void AEnemy::BreakConstraint(FName BoneName) {
	Mesh->BreakConstraint(FVector(0, 0, 100), GetActorLocation(), BoneName);
	Mesh->SetAllBodiesBelowSimulatePhysics(BoneName, true, true);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (EnemyState)
	{
	case EEnemyState::Broken:
	case EEnemyState::Off:
		Radar->SetLightColor(FColor::Transparent);
		break;
	case EEnemyState::Idle:
		Radar->SetLightColor(FColor::Blue);
		break;
	case EEnemyState::Alert:
		Radar->SetLightColor(FColor::Yellow);
		break;
	case EEnemyState::Fighting:
		Radar->SetLightColor(FColor::Red);
		break;
	default:
		break;
	}
}

void AEnemy::CheckExceedingStats()
{
	TArray<EDamageTypes> DamageTypes = MainStats.GetZeroValues();
	
	for (auto& Type : DamageTypes) {
		switch (Type)
		{
		case EDamageTypes::Physical:
			DestroyEnemy();
			break;
		case EDamageTypes::Fire:
			ResetElementalStates();

			BurnCounter = 4.f;
			ElemState = EElementalState::OnFire;
			GetWorld()->GetTimerManager().SetTimer(BurnTimer, this, &AEnemy::Burning, 1.f, true);	

			ElementalState->SetTemplate(Fire);
			ElementalState->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
			ElementalState->Activate();
			break;
		case EDamageTypes::Ice:
			ResetElementalStates();

			ElemState = EElementalState::Frozen;
			GetWorld()->GetTimerManager().SetTimer(IceTimer, this, &AEnemy::UnFreeze, 5.f, false);

			ElementalState->SetTemplate(Ice);
			ElementalState->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
			ElementalState->Activate();
			break;
		case EDamageTypes::Electric:
			ResetElementalStates();

			ElemState = EElementalState::Stunned;
			GetWorld()->GetTimerManager().SetTimer(ShockTimer, this, &AEnemy::RecoverFromStun, 3.f, false);
			if (GetController() != nullptr) {
				AEnemyAI* EnemyAI = Cast<AEnemyAI>(GetController());
				EnemyAI->Stun(true);
			}

			ElementalState->SetTemplate(Electric);
			ElementalState->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
			ElementalState->Activate();
			break;
		default:
			break;
		}
	}
} 

void AEnemy::ResetElementalStates() { //When you proc a new elemental state, the others reset
	MainStats.ResetEveryElementalStat();
	UnFreeze();
	RecoverFromStun();
	StopBurning();
}

void AEnemy::DestroyEnemy()
{
	ResetElementalStates();

	for (auto& SkelPart : SkeletalComponents) {
		if (SkelPart.IsAttached()) BreakConstraint(SkelPart.ConstraintName);
	}

	if (GetController() != nullptr) {
		AEnemyAI* EnemyAI = Cast<AEnemyAI>(GetController());
		EnemyAI->DisableAI(true);
	}
	Mesh->SetSimulatePhysics(true);
}

void AEnemy::Burning()
{
	if (BurnCounter <= 0) {
		StopBurning();
	}
	else {
		BurnCounter--;
		BurnDamage();
	}
}

void AEnemy::StopBurning()
{
	BurnCounter = 0;
	ElemState = EElementalState::None;
	ElementalState->Deactivate();
	if (GetWorld()->GetTimerManager().IsTimerActive(BurnTimer)) GetWorld()->GetTimerManager().ClearTimer(BurnTimer);
}

void AEnemy::UnFreeze()
{
	ElemState = EElementalState::None;
	ElementalState->Deactivate();
	if (GetWorld()->GetTimerManager().IsTimerActive(IceTimer)) GetWorld()->GetTimerManager().ClearTimer(IceTimer);
}

void AEnemy::RecoverFromStun()
{
	ElemState = EElementalState::None;
	ElementalState->Deactivate();
	if (GetWorld()->GetTimerManager().IsTimerActive(ShockTimer)) GetWorld()->GetTimerManager().ClearTimer(ShockTimer);
	if (GetController() != nullptr && GetState() != EEnemyState::Broken) {
		AEnemyAI* EnemyAI = Cast<AEnemyAI>(GetController());
		EnemyAI->Stun(false);
	}
}

void AEnemy::BurnDamage() 
{
	float FireDamage = MainStats.FullStats[EDamageTypes::Physical] / 10;
	MainStats.ModifyStat(EDamageTypes::Physical, FireDamage, EModifierType::Decrease);
	CheckExceedingStats();
}

void AEnemy::SetState(EEnemyState NewState, bool bForceState) 
{
	if (EnemyState != EEnemyState::Broken) {
		if(bForceState) EnemyState = NewState;
		else if(ElemState != EElementalState::Stunned) EnemyState = NewState;
	}
}

FUnownedInventory* AEnemy::GetLoot()
{
	return &Loot;
}

void AEnemy::ResetAISensing()
{
	if (GetController() != nullptr) {
		AEnemyAI* EnemyAI = Cast<AEnemyAI>(GetController());
		EnemyAI->ResetSensing();
	}
}
