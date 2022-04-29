#include "FloatingDrone.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SpotLightComponent.h"
#include "AllyDrone.h"
#include "Archer.h"
#include "FloatingDroneIA.h"
#include "Bullet.h"
#include "Sound/SoundCue.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "FloatingDroneAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Inventory.h"
#include "Items/ResourcesInventoryItem.h"

AFloatingDrone::AFloatingDrone()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("SkeletalMesh'/Game/FloatingDrone/Mesh/FloatingDrone.FloatingDrone'"));
	Mesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
	Mesh->SetSkeletalMesh(SkeletalMeshAsset.Object);
	Mesh->SetGenerateOverlapEvents(true);

	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBlueprint(TEXT("AnimBlueprint'/Game/FloatingDrone/Animations/FloatingDroneAnimBP.FloatingDroneAnimBP'"));
	if (AnimBlueprint.Object != NULL)
	{
		UAnimBlueprint* AnimBP = AnimBlueprint.Object;
		Mesh->SetAnimInstanceClass(AnimBP->GetAnimBlueprintGeneratedClass());
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> ShootSoundCue(TEXT("SoundCue'/Game/FloatingDrone/Sound/FloatingDroneShoot_Cue.FloatingDroneShoot_Cue'"));
	ShootSound = ShootSoundCue.Object;

	Radar->SetupAttachment(Mesh, FName("lightSocket"));
	Radar->SetIntensity(LightIntensity);
	Radar->SetOuterConeAngle(OuterConeAngle);
	Radar->SetAttenuationRadius(AttenuationRadius);

	Speed = 3.f;
	InFightRotationSpeed = 10.f;
	PatrolRotationSpeed = 5.f;
	FireRateMin = 0.5f;
	FireRateMax = 2.f;

	AIControllerClass = AFloatingDroneIA::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	float HP = 180.f;
	float TearResistance = -1.f;
	float FireResistance = 60.f;
	float IceResistance = 60.f;
	float ElectResistance = 60.f;

	float AccrochePhysicalDamage = 25.f;
	float AccrocheTearResistance = 20.f;

	MainStats = FDefensiveStats(GetDamageMapFromValues(HP, TearResistance, FireResistance, IceResistance, ElectResistance));

	TMap<EDamageTypes, float> AccrocheDamages = GetDamageMapFromValues(AccrochePhysicalDamage, 0.f, 0.f, 0.f, 0.f);
	FDefensiveStats  AccrocheResistances = FDefensiveStats(GetDamageMapFromValues(0.f, AccrocheTearResistance, 0.f, 0.f, 0.f));

	FSkeletalParts LeftAccroche = FSkeletalParts(AccrocheResistances, FName("limb_l"));
	LeftAccroche.SetAsWeapon(AccrocheDamages);

	FSkeletalParts RightAccroche = FSkeletalParts(AccrocheResistances, FName("limb_r"));
	RightAccroche.SetAsWeapon(AccrocheDamages);

	FDefensiveStats AntenneStats = FDefensiveStats(GetDamageMapFromValues(0.f, 1.f, 0.f, 0.f, 0.f));
	FSkeletalParts RightAntenne = FSkeletalParts(AntenneStats, FName("antenne_r"));
	RightAntenne.SetDamageModifier(0.5);
	FSkeletalParts LeftAntenne = FSkeletalParts(AntenneStats, FName("antenne_l"));
	LeftAntenne.SetDamageModifier(0.5);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PredictAsset(TEXT("ParticleSystem'/Game/Particles/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Elemental/Fire/P_ElementalFire_Lg.P_ElementalFire_Lg'"));
	PredictShoot = PredictAsset.Object;

	SkeletalComponents.Add(LeftAccroche);
	SkeletalComponents.Add(RightAccroche);
	SkeletalComponents.Add(RightAntenne);
	SkeletalComponents.Add(LeftAntenne);

	ShootSide = ESide::Right;
}

void AFloatingDrone::BeginPlay()
{
	Super::BeginPlay();

	LightMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(Mesh->GetMaterialIndex(FName("M_Light_Eye"))), this);
	AnimInstance = Cast<UFloatingDroneAnimInstance>(Mesh->GetAnimInstance());

	int32 NumOfCopperFrag = FMath::RandRange(10, 20);
	int32 NumOfWires = FMath::RandRange(12, 18);
	int32 NumOfMetalPlates = FMath::RandRange(0, 5);
	int32 NumOfElectricalComponents = FMath::RandRange(5, 8);

	Loot.UpdateResource(EResourceTypes::CopperFragment, NumOfCopperFrag, EModifierType::Increase);
	Loot.UpdateResource(EResourceTypes::Wire, NumOfWires, EModifierType::Increase);
	Loot.UpdateResource(EResourceTypes::MetalPlate, NumOfMetalPlates, EModifierType::Increase);
	Loot.UpdateResource(EResourceTypes::ElectronicComponent, NumOfElectricalComponents, EModifierType::Increase);
}

void AFloatingDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (GetState())
	{
	case EEnemyState::Idle:
		LightMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Blue);
		Mesh->SetMaterial(Mesh->GetMaterialIndex(FName("M_Light_Eye")), LightMaterial);
		break;
	case EEnemyState::Fighting:
		LightMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Red);
		Mesh->SetMaterial(Mesh->GetMaterialIndex(FName("M_Light_Eye")), LightMaterial);
		break;
	case EEnemyState::Alert:
		LightMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Yellow);
		Mesh->SetMaterial(Mesh->GetMaterialIndex(FName("M_Light_Eye")), LightMaterial);
		break;
	case EEnemyState::Broken:
	case EEnemyState::Off:
		LightMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Transparent);
		Mesh->SetMaterial(Mesh->GetMaterialIndex(FName("M_Light_Eye")), LightMaterial);
		break;
	default:
		break;
	}
}

void AFloatingDrone::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if (EnemyState != EEnemyState::Fighting && Other != this && Other->GetInstigator()) {	
		if (Other->GetInstigator()->IsA(AArcher::StaticClass()) || Other->GetInstigator()->IsA(AAllyDrone::StaticClass())) {
			AFloatingDroneIA* AI = Cast<AFloatingDroneIA>(GetController());	
			if (AI) AI->HitNotify(Other->GetInstigator());
		}
	}
}

void AFloatingDrone::DestroyEnemy()
{
	
	GetWorld()->GetTimerManager().ClearTimer(PredictShootTimer);
	if (CurrentPredictLight != nullptr) CurrentPredictLight->DeactivateSystem();
	Super::DestroyEnemy();
}

void AFloatingDrone::Shoot()
{
	if (EnemyState == EEnemyState::Broken) {
		if (CurrentPredictLight != nullptr) CurrentPredictLight->DeactivateSystem();	
		return;
	}

	if ((ShootSide == ESide::Right && !CheckBrokenAccroche(FName("limb_r"))) || (ShootSide == ESide::Left && !CheckBrokenAccroche(FName("limb_l"))))
	{
		if (EnemyState == EEnemyState::Fighting) {
			FRotator Rotazione = GetActorRotation();
			FVector Posizione;

			FName SkelName;
			FName CanonName;

			if (ShootSide == ESide::Right) {
				SkelName = FName("limb_r");
				CanonName = FName("cannon_rSocket");
				ShootSide = ESide::Left;
			}
			else {
				SkelName = FName("limb_l");
				CanonName = FName("cannon_lSocket");
				ShootSide = ESide::Right;
			}

			FAggressiveStats Damage;

			for (auto& SkelPart : SkeletalComponents) {
				if (SkelPart.ConstraintName == SkelName) Damage = SkelPart.DamageStats;
			}

			Posizione = Mesh->GetSocketLocation(CanonName);
			if (Target != nullptr) Rotazione = (Target->GetActorLocation() - Mesh->GetSocketLocation(CanonName)).Rotation();

			if (ShootSide == ESide::Right) AnimInstance->ShootSX(); //Inverted because already changed for the next shoot
			else AnimInstance->ShootDX();

			FActorSpawnParameters Spawner;
			Spawner.bNoFail = true;
			Spawner.Owner = this;
			Spawner.Instigator = this;

			ABullet* Projectile = GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(), Posizione, Rotazione, Spawner);
			Projectile->SetDamage(Damage, 40.f);
			Projectile->Shoot();

			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, Posizione);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShootSpark, Posizione, FRotator::ZeroRotator, FVector(0.5f, 0.5f, 0.5f));
		}
	}
	if (CurrentPredictLight != nullptr) CurrentPredictLight->DestroyComponent();
	StartShooting();
}

void AFloatingDrone::PredictShot()
{
	bool rightBroken = CheckBrokenAccroche(FName("limb_r"));
	bool leftBroken = CheckBrokenAccroche(FName("limb_l"));
	if (rightBroken && leftBroken) return;

	if (ShootSide == ESide::Right)
	{
		if (!rightBroken) {
			if(ElemState != EElementalState::Stunned && GetState() == EEnemyState::Fighting) CurrentPredictLight = UGameplayStatics::SpawnEmitterAttached(PredictShoot, Mesh, FName("cannon_rSocket"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), EAttachLocation::SnapToTarget);
			GetWorld()->GetTimerManager().SetTimer(PredictShootTimer, this, &AFloatingDrone::Shoot, 1.f, false);
		}
		else {
			ShootSide = ESide::Left;
			PredictShot();
		}
	}
	else
	{
		if (!leftBroken) {
			if (ElemState != EElementalState::Stunned && GetState() == EEnemyState::Fighting) CurrentPredictLight = UGameplayStatics::SpawnEmitterAttached(PredictShoot, Mesh, FName("cannon_lSocket"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), EAttachLocation::SnapToTarget);
			GetWorld()->GetTimerManager().SetTimer(PredictShootTimer, this, &AFloatingDrone::Shoot, 1.f, false);
		}
		else {
			ShootSide = ESide::Right;
			PredictShot();
		}
	}
}

bool AFloatingDrone::CheckBrokenAccroche(FName AccrocheName)
{
	for (auto& SkelPart : SkeletalComponents) {
		if (SkelPart.ConstraintName == AccrocheName) {
			if (!SkelPart.IsAttached()) return true;
			else return false;
		}
	}
	return true;
}

void AFloatingDrone::StartShooting()
{
	float RandomNumber = FMath::FRandRange(FireRateMin, FireRateMax);
	GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &AFloatingDrone::PredictShot, RandomNumber, false);
}

void AFloatingDrone::EnterCombatMode()
{
	AnimInstance->SetIsFighting(true);
}

void AFloatingDrone::ExitCombatMode()
{
	AnimInstance->SetIsFighting(false);

	if (GetWorld()->GetTimerManager().IsTimerActive(ShootTimer)) GetWorld()->GetTimerManager().ClearTimer(ShootTimer);
}