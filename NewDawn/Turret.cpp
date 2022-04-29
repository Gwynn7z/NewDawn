#include "Turret.h"
#include "Components/SkeletalMeshComponent.h"
#include "TurretAI.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SpotLightComponent.h"
#include "AllyDrone.h"
#include "Archer.h"
#include "Bullet.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimBlueprint.h"
#include "TurretAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Inventory.h"
#include "Items/ResourcesInventoryItem.h"

ATurret::ATurret() 
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("SkeletalMesh'/Game/Turret/Mesh/Turret.Turret'"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetSkeletalMesh(SkeletalMeshAsset.Object);
	Mesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
	Mesh->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	Mesh->SetGenerateOverlapEvents(true);
	
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBlueprint(TEXT("AnimBlueprint'/Game/Turret/Animations/TurretAnimBP.TurretAnimBP'"));
	if (AnimBlueprint.Object != NULL)
	{
		UAnimBlueprint* AnimBP = AnimBlueprint.Object;
		Mesh->SetAnimInstanceClass(AnimBP->GetAnimBlueprintGeneratedClass());
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> ShootSoundCue(TEXT("SoundCue'/Game/Turret/Sound/Shoot_Cue.Shoot_Cue'"));
	ShootSound = ShootSoundCue.Object;

	Radar->SetupAttachment(Mesh, FName("lightSocket"));
	Radar->SetIntensity(LightIntensity);
	Radar->SetOuterConeAngle(OuterConeAngle);
	Radar->SetAttenuationRadius(AttenuationRadius);


	AIControllerClass = ATurretAI::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	float HP = 500.f;
	float TearResistance = -1.f;
	float FireResistance = 60.f;
	float IceResistance = -1.f;
	float ElectResistance = -1.f;

	float CannonDamage = 30.f;
	float CannonResistance = 60.f;
	float HeatsinkIncreaser = 2.5f;
	float CannonIncreaser = 0.75f;

	MainStats = FDefensiveStats(GetDamageMapFromValues(HP, TearResistance, FireResistance, IceResistance, ElectResistance));

	TMap<EDamageTypes, float> Damages = GetDamageMapFromValues(CannonDamage, 0.f, 0.f, 0.f, 0.f);

	FDefensiveStats CannonStats = FDefensiveStats(GetDamageMapFromValues(0.f, CannonResistance, 0.f, 0.f, 0.f));

	FSkeletalParts LeftCannon = FSkeletalParts(CannonStats, FName("cannon_l"));
	LeftCannon.SetDamageModifier(CannonIncreaser);
	LeftCannon.SetAsWeapon(Damages);

	FSkeletalParts RightCannon = FSkeletalParts(CannonStats, FName("cannon_r"));
	RightCannon.SetDamageModifier(CannonIncreaser);
	RightCannon.SetAsWeapon(Damages);

	FSkeletalParts HeatSink = FSkeletalParts(FDefensiveStats(), FName("heatsink"));
	HeatSink.SetDamageModifier(HeatsinkIncreaser);

	SkeletalComponents.Add(LeftCannon);
	SkeletalComponents.Add(RightCannon);
	SkeletalComponents.Add(HeatSink);

	BlendControl = false;
	CanShoot = false;
	isOverHeat = false;

	OverHeatTimerValue = 5.f;
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UTurretAnimInstance>(Mesh->GetAnimInstance());

	int32 NumOfCopperFrag = FMath::RandRange(10, 20);
	int32 NumOfWires = FMath::RandRange(5, 10);
	int32 NumOfMetalPlates = FMath::RandRange(5, 10);
	int32 NumOfElectricalComponents = FMath::RandRange(0, 3);

	Loot.UpdateResource(EResourceTypes::CopperFragment, NumOfCopperFrag, EModifierType::Increase);
	Loot.UpdateResource(EResourceTypes::Wire, NumOfWires, EModifierType::Increase);
	Loot.UpdateResource(EResourceTypes::MetalPlate, NumOfMetalPlates, EModifierType::Increase);
	Loot.UpdateResource(EResourceTypes::ElectronicComponent, NumOfElectricalComponents, EModifierType::Increase);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	switch (EnemyState)
	{
	case EEnemyState::Broken:
		SetFighting(false);
		AnimInstance->StopShooting();
	case EEnemyState::Idle:
		if(!AnimInstance->IsShooting()) SetFighting(false);
		break;
	case EEnemyState::Alert:
	case EEnemyState::Fighting:
		SetFighting(true);
		break;
	default:
		break;
	}
}

void ATurret::CheckExceedingStats()
{
	TArray<EDamageTypes> DamageTypes = MainStats.GetZeroValues();

	for (auto& Type : DamageTypes) {
		switch (Type) {
		case EDamageTypes::Fire:
			AnimInstance->StopShooting();
			TriggerOverHeat();
		}
	}

	Super::CheckExceedingStats();
}

void ATurret::ShootLeft()
{
	for (auto& Part : SkeletalComponents) {
		if (Part.ConstraintName == FName("cannon_l")) {
			if (Part.IsAttached()) {
				Shoot(Part);
			}
		}
	}
}

void ATurret::ShootRight()
{
	for (auto& Part : SkeletalComponents) {
		if (Part.ConstraintName == FName("cannon_r")) {
			if (Part.IsAttached()) {
				Shoot(Part);
			}
		}
	}
}

void ATurret::StartShooting()
{
	if (!isOverHeat && !AnimInstance->IsShooting()) AnimInstance->Shoot();
	CanShoot = true;
}

void ATurret::StopShooting()
{
	CanShoot = false;
}

void ATurret::HeatUp()
{
	HeatValue++;
	if (HeatValue >= 5) AnimInstance->LastShoot();
}

void ATurret::TriggerOverHeat()
{
	isOverHeat = true;
	AnimInstance->SetIsOverheaten(true);
	GetWorld()->GetTimerManager().SetTimer(OverHeatTimer, this, &ATurret::CoolDown, OverHeatTimerValue);
}

void ATurret::CoolDown()
{
	AnimInstance->SetIsOverheaten(false);
	isOverHeat = false;
	HeatValue = 0;

	if (CanShoot) StartShooting();
}

void ATurret::SetCannonRotation(FRotator Rotation)
{
	Rotation.Pitch = FMath::Clamp(Rotation.Pitch, -40.f, 40.f);
	FRotator AdjRotation = FRotator(0, Rotation.Yaw + 90.f, Rotation.Pitch); //Pitch in Roll and Yaw + 90 for Mesh Adjustments
	AnimInstance->SetCannonRotation(AdjRotation, Mesh->GetBoneQuaternion(FName("corp"), EBoneSpaces::WorldSpace).Rotator());
}

FVector ATurret::GetCannonLocation()
{
	FVector Location = Mesh->GetBoneLocation(FName("corp"), EBoneSpaces::WorldSpace);
	return Location;
}

FRotator ATurret::GetCannonRotation()
{
	FRotator Rotazione = Mesh->GetBoneQuaternion(FName("corp"), EBoneSpaces::WorldSpace).Rotator();
	FRotator DefRotation = FRotator(Rotazione.Roll, Rotazione.Yaw - 90.f, Rotazione.Pitch); //God pig adjustments
	return DefRotation;
}

void ATurret::SetFighting(bool isFighting)
{
	if (BlendControl != isFighting) {
		BlendControl = isFighting;
		AnimInstance->SetIsFighting(isFighting);
	}
}

void ATurret::Shoot(FSkeletalParts Side)
{
	FName SkelName = Side.ConstraintName;

	FRotator Rotation = GetCannonRotation();
	FVector Posizione = Mesh->GetSocketLocation(*(SkelName.ToString() + "Socket"));

	FActorSpawnParameters Spawner;
	Spawner.bNoFail = true;
	Spawner.Owner = this;
	Spawner.Instigator = this;

	ABullet* Projectile = GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(), Posizione, Rotation, Spawner);
	Projectile->SetDamage(Side.DamageStats, 100.f);
	Projectile->Shoot();	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShootSpark, Posizione, FRotator::ZeroRotator, FVector(0.3f, 0.3f, 0.3f));
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, Posizione);
}

