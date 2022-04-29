#include "AllyDrone.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SpotLightComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/StaticMeshActor.h"
#include "Battery.h"
#include "Items.h"
#include "Shot.h"
#include "Weight.h"
#include "ControlPanel.h"
#include "Archer.h"
#include "AllyDroneAI.h"
#include "AllyDroneAnimInstance.h"
#include "Electrodes.h"
#include "DefensesBars.h"
#include "Menu.h"
#include "Items/Inventory.h"
#include "Blueprint/UserWidget.h"
#include "DroneWeaponsWidget.h"
#include "DroneAbilities/DroneAbility.h"
#include "DroneAbilities/ShotAbility.h"
#include "DroneAbilities/TwinAbility.h"
#include "DroneAbilities/DroneUpgrade.h"
#include "InteractWidget.h"
#include "Enemy.h"
#include "Workbench.h"
#include "EnemyAI.h"

AAllyDrone::AAllyDrone()
{
	PrimaryActorTick.bCanEverTick = true;

	Drone = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Drone);
	SetCollisionPreset();
	Drone->SetGenerateOverlapEvents(true);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BaseMeshAsset1(TEXT("SkeletalMesh'/Game/Drone/Mesh/AllyDrone.AllyDrone'"));
	Drone->SetSkeletalMesh(BaseMeshAsset1.Object);
	Drone->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBlueprint(TEXT("AnimBlueprint'/Game/Drone/Animations/AllyDroneAnimBP.AllyDroneAnimBP'"));
	if (AnimBlueprint.Object != NULL)
	{
		UAnimBlueprint* AnimBP = AnimBlueprint.Object;
		Drone->SetAnimInstanceClass(AnimBP->GetAnimBlueprintGeneratedClass());
	}

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = armLenght;
	CameraSpringArm->SetRelativeRotation(SpringArmRotation);
	CameraSpringArm->SetRelativeLocation(SpringArmLocation);
	CameraSpringArm->bInheritPitch = true;
	CameraSpringArm->bInheritRoll = false;
	CameraSpringArm->bInheritYaw = true;
	CameraSpringArm->bEnableCameraRotationLag = true;
	CameraSpringArm->CameraRotationLagSpeed = 20.f;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraSpringArm);
	ThirdPersonCamera->PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
	ThirdPersonCamera->PostProcessSettings.AutoExposureBias = 13.5;
	ThirdPersonCamera->SetRelativeRotation(CameraRotation);
	ThirdPersonCamera->SetRelativeLocation(CameraLocation);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->AttachToComponent(Drone, FAttachmentTransformRules::KeepRelativeTransform, FName("CameraSocket"));
	FirstPersonCamera->PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
	FirstPersonCamera->PostProcessSettings.AutoExposureBias = 13.5;

	Light = CreateDefaultSubobject<USpotLightComponent>(TEXT("Light"));
	Light->AttachToComponent(Drone, FAttachmentTransformRules::KeepRelativeTransform, FName("LightSocket"));
	Light->SetRelativeLocation(LightLocation);
	Light->SetAttenuationRadius(LightRange);
	Light->SetIntensity(0.f);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> SparksAsset(TEXT("ParticleSystem'/Game/Particles/StarterContent/Particles/P_Sparks.P_Sparks'"));
	if (SparksAsset.Succeeded()) Sparks = SparksAsset.Object;

	//static ConstructorHelpers::FObjectFinder<USoundCue> ShootSoundCue(TEXT("SoundCue'/Game/FloatingDrone/Sound/FloatingDroneShoot_Cue.FloatingDroneShoot_Cue'"));
	//ShootSound = ShootSoundCue.Object;

	OverrideMaxCharge = 6.f;
	MaxEnergyValue = 100.f;
	ElectrodesEnergyCost = 30.f;
	ShotEnergyCost = 10.f;

	ForwardThrustPower = 6.f;
	ForwardThrustResistance = 20.f;
	ForwardOverheat = 1000.f;
	CurrentForwardThrust = 0;
	TempOverheat = ForwardOverheat;

	RightThrustPower = 6.f;
	RightThrustResistance = 20.f;
	RightOverheat = 800.f;
	CurrentRightThrust = 0;

	UpThrustPower = 6.f;
	UpThrustResistance = 12.f;
	UpOverheat = 500.f;
	CurrentUpThrust = 0;

	YawSpeed = 0.5f;
	RollSpeed = 1.f;
	PitchSpeed = 0.25f;
	MaxPitch = 40.f;
	InvertAxis = false;

	DroneState = EAllyDroneState::Disabled;

	AIControllerClass = AAllyDroneAI::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	float HP = 120.f;
	float TearResistance = -1.f;
	float FireResistance = 40.f;
	float IceResistance = 40.f;
	float ElectResistance = 20.f;

	float CannonPhysical = 36.f;
	float CannonElectric = 20.f;
	float ElectrodesPhysical = 120.f;
	float ElectrodesElectric = 100.f;

	DefensiveStats = FDefensiveStats(GetDamageMapFromValues(HP, TearResistance, FireResistance, IceResistance, ElectResistance));

	CannonDamage = FAggressiveStats(GetDamageMapFromValues(CannonPhysical, 0.f, 0.f, 0.f, CannonElectric));
	ElectrodesDamage = FAggressiveStats(GetDamageMapFromValues(ElectrodesPhysical, 0.f, 0.f, 0.f, ElectrodesElectric));

	CollisionDamage = GetDamageMapFromValues(20.f, 0.f, 0.f, 0.f, 0.f);
	DrainDamage = GetDamageMapFromValues(0.5f, 0.f, 0.f, 0.f, 0.f);

	static ConstructorHelpers::FClassFinder<UUserWidget> WeaponAsset(TEXT("/Game/HUD/DroneWeapons"));
	WeaponAssetClass = WeaponAsset.Class;

	DroneStillAlive.Add(EResourceTypes::CopperFragment, 8);
	DroneStillAlive.Add(EResourceTypes::Wire, 5);
	DroneStillAlive.Add(EResourceTypes::MetalPlate, 2);
	DroneStillAlive.Add(EResourceTypes::ElectronicComponent, 1);

	DroneDestroyed.Add(EResourceTypes::CopperFragment, 20);
	DroneDestroyed.Add(EResourceTypes::Wire, 6);
	DroneDestroyed.Add(EResourceTypes::MetalPlate, 4);
	DroneDestroyed.Add(EResourceTypes::ElectronicComponent, 5);

	UpgradeSlots.Add(nullptr);
	UpgradeSlots.Add(nullptr);

	bWorkbenchIsOverlapping = false;
}


void AAllyDrone::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = Cast<UAllyDroneAnimInstance>(Drone->GetAnimInstance());
	WeaponsWidget = CreateWidget<UDroneWeaponsWidget>(GetWorld(), WeaponAssetClass);
	WeaponsWidget->HideImages();

	EnergyValue = MaxEnergyValue;

	ThirdPersonCamera->Activate();
	FirstPersonCamera->Deactivate();
	
	OverrideCharge = 0;
	GrabbedItem = nullptr;
	CurrentlyInteractingActor = nullptr;
	CurrentlyGrabbingActor = nullptr;
	CanShoot = true;

	if (InvertAxis) PitchSpeed *= -1;

	if(GetController() != NULL) DroneAI = Cast<AAllyDroneAI>(GetController());

	RightAbility = nullptr;
	LeftAbility = nullptr;

	UShotAbility* Shot = NewObject<UShotAbility>();
	UTwinAbility* Twin = NewObject<UTwinAbility>();
	UnlockedAbilities.Add(Shot);
	UnlockedAbilities.Add(Twin);

	UDroneUpgrade* EnergyDrain = NewObject<UDroneUpgrade>();
	UDroneUpgrade* EnergySaver = NewObject<UDroneUpgrade>();
	EnergyDrain->SetUpgrade(EDroneUpgrade::EnergyDrain);
	EnergySaver->SetUpgrade(EDroneUpgrade::EnergySaver);

	UnlockedUpgrades.Add(EnergyDrain);
	UnlockedUpgrades.Add(EnergySaver);
}

void AAllyDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (DroneState) {
	case EAllyDroneState::Active:
	{
		//Move Forward
		AddActorLocalOffset(FVector(CurrentForwardThrust * DeltaTime, CurrentRightThrust * DeltaTime, CurrentUpThrust * DeltaTime), true);
		SetActorRotation(CurrentRotation);
	}
	case EAllyDroneState::Disabled:
		break;
	case EAllyDroneState::Broken:
		break;
	}
}

void AAllyDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AAllyDrone::Interact);
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AAllyDrone::Grab);
	PlayerInputComponent->BindAction("ToggleLight", IE_Pressed, this, &AAllyDrone::ToggleLight);

	PlayerInputComponent->BindAction("SwapController", IE_Pressed, this, &AAllyDrone::SwapToArcher);
	PlayerInputComponent->BindAction("CameraSwitch", IE_Pressed, this, &AAllyDrone::CameraSwitch);

	PlayerInputComponent->BindAxis("Turn", this, &AAllyDrone::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AAllyDrone::Pitch);

	PlayerInputComponent->BindAxis("ForwardThrust", this, &AAllyDrone::ForwardThrust);
	PlayerInputComponent->BindAxis("RightThrust", this, &AAllyDrone::RightThrust);
	PlayerInputComponent->BindAxis("UpThrust", this, &AAllyDrone::UpThrust);
	PlayerInputComponent->BindAxis("NoLimitSpeed", this, &AAllyDrone::SpeedUp);

	PlayerInputComponent->BindAction("LeftClickAbility", EInputEvent::IE_Pressed, this, &AAllyDrone::LeftClickAbility);
	PlayerInputComponent->BindAction("LeftClickAbility", EInputEvent::IE_Released, this, &AAllyDrone::LeftReleaseAbility);
	PlayerInputComponent->BindAction("RightClickAbility", EInputEvent::IE_Pressed, this, &AAllyDrone::RightClickAbility);
	PlayerInputComponent->BindAction("RightClickAbility", EInputEvent::IE_Released, this, &AAllyDrone::RightReleaseAbility);
	PlayerInputComponent->BindAxis("LeftHoldAbility", this, &AAllyDrone::LeftHoldAbility);
	PlayerInputComponent->BindAxis("RightHoldAbility", this, &AAllyDrone::RightHoldAbility);
	
	PlayerInputComponent->BindAxis("Drain", this, &AAllyDrone::DrainEnergy);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AAllyDrone::OpenMenu);
}

void AAllyDrone::SetArcher(AArcher* NewArcher)
{
	Archer = NewArcher; 
	if (DroneAI) DroneAI->SetArcher(Archer);
}

void AAllyDrone::OpenMenu()
{
	Menu->SetShow(true);

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(Menu->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	GetWorld()->GetFirstPlayerController()->SetInputMode(Mode);

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
}

void AAllyDrone::SetDefaultBarValues()
{
	if (Bars && DroneAI->GetAIState() == EAllyDroneIAState::Piloting)
	{
		Bars->SetMainBarMaxValue(DefensiveStats.FullStats[EDamageTypes::Physical]);
		UpdateHPBar();

		Bars->SetSecondaryBarMaxValue(MaxEnergyValue);
		UpdateEnergyBar();

		Bars->SetEnergyBarMaxValue(OverrideMaxCharge);
		UpdateOverrideBar();

		Bars->HideSecondaryBar(false);
	}
}

void AAllyDrone::UpdateHPBar()
{
	if (Bars && DroneAI->GetAIState() == EAllyDroneIAState::Piloting) Bars->SetMainBarValue(DefensiveStats.Stats[EDamageTypes::Physical]);
}

void AAllyDrone::UpdateEnergyBar()
{
	if(Bars && DroneAI->GetAIState() == EAllyDroneIAState::Piloting) Bars->SetSecondaryBarValue(EnergyValue);
}

void AAllyDrone::UpdateOverrideBar()
{
	if (Bars && DroneAI->GetAIState() == EAllyDroneIAState::Piloting) {
		if(OverrideCharge != 0.f) Bars->HideEnergyBar(false);
		else Bars->HideEnergyBar(true);
		Bars->SetEnergyBarValue(OverrideCharge);
	}
}

void AAllyDrone::SetHUDBarWidget(UDefensesBars* NewBars)
{
	Bars = NewBars;
}

void AAllyDrone::SetHUDInteractWidget(UInteractWidget* NewInteract)
{
	InteractWidget = NewInteract;
}

void AAllyDrone::ShareMenu(UMenu* NewMenu)
{
	Menu = NewMenu;
}

void AAllyDrone::SwapToArcher() 
{
	if (Archer != nullptr)
	{
		FHitResult Hit;
		FVector Start = GetActorLocation();
		FVector End = Archer->FloatingDroneLocation->GetComponentLocation();
		FCollisionQueryParams CollisionParam;
		CollisionParam.AddIgnoredActor(this);
		CollisionParam.AddIgnoredActor(Archer);

		if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, CollisionParam)) {
			Stop();
			Detach();
			SetActorRotation((Archer->FloatingDroneLocation->GetComponentLocation() - GetActorLocation()).Rotation());
			if(DroneAI) DroneAI->GotoIdle();
			if (GetWorld()->GetTimerManager().IsTimerActive(InteractTimer)) GetWorld()->GetTimerManager().ClearTimer(InteractTimer);
			AnimInstance->SetIsOpen(false);

			if (InteractWidget) InteractWidget->SetInteractText("");
			if (WeaponsWidget) if (WeaponsWidget->IsInViewport()) WeaponsWidget->RemoveFromViewport();
		}
	}
}

UDroneAbility* AAllyDrone::GetAbility(ESide Side)
{
	switch (Side) {
	case ESide::Right:
		return RightAbility;
		break;
	case ESide::Left:
		return LeftAbility;
		break;
	default:
		return nullptr;
	}
}

TArray<UDroneAbility*> AAllyDrone::GetUnlockedAbilities()
{
	return UnlockedAbilities;
}

void AAllyDrone::AddToSlot(UDroneUpgrade* NewUpgrade)
{
	int32 Index = 0;
	for (int32 i = 0; i < UpgradeSlots.Num(); i++) {
		if (UpgradeSlots[i] != nullptr) {
			if (UpgradeSlots[i]->GetUpgrade() == NewUpgrade->GetUpgrade()) return;
		}
		else {
			Index = i;
			break;
		}
	}
	UpgradeSlots[Index] = NewUpgrade;
}

void AAllyDrone::RemoveFromSlot(UDroneUpgrade* NewUpgrade)
{
	for (int32 i = 0; i < UpgradeSlots.Num(); i++) {
		if (UpgradeSlots[i] != nullptr) {
			if (UpgradeSlots[i]->GetUpgrade() == NewUpgrade->GetUpgrade()) {
				UpgradeSlots.RemoveAt(i);
				UpgradeSlots.Add(nullptr);
				return;
			}
		}
	}
}

TArray<UDroneUpgrade*> AAllyDrone::GetUpgradeSlots()
{
	return UpgradeSlots;
}

TArray<UDroneUpgrade*> AAllyDrone::GetUnlockedUpgrades()
{
	return UnlockedUpgrades;
}

void AAllyDrone::SetAbility(ESide Side, UDroneAbility* NewAbility)
{
	switch (Side)
	{
	case ESide::Right:
		if (LeftAbility != nullptr && LeftAbility->GetDroneAbility() == NewAbility->GetDroneAbility())
		{
			LeftAbility = RightAbility;
			if (WeaponsWidget) WeaponsWidget->SetAbility(ESide::Left, LeftAbility);
		}
		RightAbility = NewAbility;
		break;
	case ESide::Left:
		if (RightAbility != nullptr && RightAbility->GetDroneAbility() == NewAbility->GetDroneAbility())
		{
			RightAbility = LeftAbility;
			if (WeaponsWidget) WeaponsWidget->SetAbility(ESide::Right, RightAbility);
		}
		LeftAbility = NewAbility;
		break;
	default:
		break;
	}
	if (WeaponsWidget) WeaponsWidget->SetAbility(Side, NewAbility);
}

void AAllyDrone::AddFightingEnemy(AEnemy* NewEnemy)
{
	if (!FightingEnemies.Contains(NewEnemy))
	{
		FightingEnemies.Add(NewEnemy);
	}
}

void AAllyDrone::RemoveFightingEnemy(AEnemy* NewEnemy)
{
	if (FightingEnemies.Contains(NewEnemy))
	{
		FightingEnemies.Remove(NewEnemy);
	}
}

void AAllyDrone::Pilot() {
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	GetWorld()->GetFirstPlayerController()->Possess(this);
	DroneAI->DisableAI();
	if (!GetWorld()->GetTimerManager().IsTimerActive(InteractTimer)) GetWorld()->GetTimerManager().SetTimer(InteractTimer, this, &AAllyDrone::InteractCycle, 0.5f, true);
	AnimInstance->SetIsOpen(true);

	SetDefaultBarValues();
	WeaponsWidget->AddToViewport();
	Menu->EnableInventory(false);
}

void AAllyDrone::SetDroneState(EAllyDroneState State)
{
	DroneState = State;
}

void AAllyDrone::SetCollisionPreset()
{
	Drone->SetCollisionProfileName(FName("Pawn"));
	Drone->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Drone->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

// ---------------------------- HP FUNCTIONS --------------------------------------
void AAllyDrone::Heal(float Amount)
{
	DefensiveStats.ModifyStat(EDamageTypes::Physical, Amount, EModifierType::Increase);
	UpdateHPBar();

	if (GetLifeState() == EAllyDroneState::Broken) {
		if(SparkComponent) SparkComponent->DestroyComponent();
		SetCollisionPreset();
		DroneAI->ReviveDrone();
		DroneState = EAllyDroneState::Disabled;
	}
}

bool AAllyDrone::IsFullHP()
{
	if (DefensiveStats.FullStats[EDamageTypes::Physical] == DefensiveStats.Stats[EDamageTypes::Physical]) return true;
	else return false;
}


void AAllyDrone::GetHit(TMap<EDamageTypes, float> Damages)
{
	if (DroneState != EAllyDroneState::Broken) {
		DefensiveStats.ModifyStats(Damages, EModifierType::Decrease);
		for (auto& Damage : DefensiveStats.GetZeroValues()) {
			switch (Damage)
			{
			case EDamageTypes::Physical:
				if (WeaponsWidget) if (WeaponsWidget->IsInViewport()) WeaponsWidget->RemoveFromViewport();
				AnimInstance->SetIsOpen(false);
				Drone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				DroneAI->BreakDrone();
				SparkComponent = UGameplayStatics::SpawnEmitterAttached(Sparks, Drone, FName("SparkSocket"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(0.5f, 0.5f, 0.5f), EAttachLocation::SnapToTarget);
				Light->SetIntensity(0.f);
				Stop();
				Detach();
				for (auto& Enemy : FightingEnemies) Enemy->ResetAISensing();		
				break;
			case EDamageTypes::Tear:
				break;
			case EDamageTypes::Fire:
				break;
			case EDamageTypes::Ice:
				break;
			case EDamageTypes::Electric:
				break;
			default:
				break;
			}
		}
		UpdateHPBar();
	}
}

TTuple<float, float> AAllyDrone::GetHPForBar()
{
	TTuple<float, float> Tuple = MakeTuple(DefensiveStats.FullStats[EDamageTypes::Physical], DefensiveStats.Stats[EDamageTypes::Physical]);
	return Tuple;
}

void AAllyDrone::Break()
{
	TMap<EDamageTypes, float> OneShot = GetVoidDamageMap();
	OneShot[EDamageTypes::Physical] = DefensiveStats.FullStats[EDamageTypes::Physical];
	GetHit(OneShot);
}

// ---------------------------- FUNCTIONS --------------------------------------
void AAllyDrone::ToggleLight()
{
	if (Light->Intensity == 0.f) Light->SetIntensity(LightIntensity);
	else Light->SetIntensity(0.f);
}

// ---------------------------- ATTACK FUNCTIONS --------------------------------------

void AAllyDrone::LeftClickAbility()
{
	if (LeftAbility == nullptr) return;
	switch (LeftAbility->GetDroneAbility())
	{
	case EAllyDroneAbilities::Shot:
		Shoot();
		break;
	default:
		break;
	}
}

void AAllyDrone::LeftReleaseAbility()
{
	if (LeftAbility == nullptr) return;
	switch (LeftAbility->GetDroneAbility())
	{
	case EAllyDroneAbilities::Electrodes:
		ActivateElectrodes();
		break;
	default:
		break;
	}
}

void AAllyDrone::RightClickAbility()
{
	if (RightAbility == nullptr) return;
	switch (RightAbility->GetDroneAbility())
	{
	case EAllyDroneAbilities::Shot:
		Shoot();
		break;
	default:
		break;
	}
}

void AAllyDrone::RightReleaseAbility()
{
	if (RightAbility == nullptr) return;
	switch (RightAbility->GetDroneAbility())
	{
	case EAllyDroneAbilities::Electrodes:
		ActivateElectrodes();
		break;
	default:
		break;
	}
}

void AAllyDrone::LeftHoldAbility(float Value)
{
	if (LeftAbility == nullptr) return;
	switch (LeftAbility->GetDroneAbility())
	{
	case EAllyDroneAbilities::Electrodes:
		AimElectrodes(Value);
		break;
	default:
		break;
	}
}


void AAllyDrone::RightHoldAbility(float Value)
{
	if (RightAbility == nullptr) return;
	switch (RightAbility->GetDroneAbility())
	{
	case EAllyDroneAbilities::Electrodes:
		AimElectrodes(Value);
		break;
	default:
		break;
	}
}


void AAllyDrone::Shoot()
{
	if(CanShoot && GetEnergyLeft() >= ShotEnergyCost)
	{
		AnimInstance->Shoot();
		CanShoot = false;
		UseEnergy(ShotEnergyCost);

		FActorSpawnParameters Spawner;
		Spawner.bNoFail = true;
		Spawner.Owner = this;
		Spawner.Instigator = this;

		FVector Posizione = Drone->GetSocketLocation(FName("cannonSocket"));
		FRotator Rotazione = Drone->GetSocketQuaternion(FName("cannonSocket")).Rotator();

		AShot* Projectile = GetWorld()->SpawnActor<AShot>(AShot::StaticClass(), Posizione, Rotazione, Spawner);
		Projectile->SetDamage(CannonDamage, 50.f);
		Projectile->Shoot();

		//UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, Posizione);
		GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &AAllyDrone::RechargeShot, 0.5f, false);
	}
}

void AAllyDrone::RechargeShot()
{
	CanShoot = true;
}

void AAllyDrone::AimElectrodes(float Value)
{
	if(Value > 0 && CurrentElectrode == nullptr && GetEnergyLeft() >= ElectrodesEnergyCost){
		FActorSpawnParameters Spawner;
		Spawner.bNoFail = true;
		Spawner.Owner = this;
		Spawner.Instigator = this;

		FVector Posizione = (Drone->GetSocketLocation(FName("electrodes_sx")) + Drone->GetSocketLocation(FName("electrodes_dx"))) / 2;
		FRotator Rotazione = GetActorRotation();

		AElectrodes* Electrode = GetWorld()->SpawnActor<AElectrodes>(AElectrodes::StaticClass(), Posizione, Rotazione, Spawner);
		Electrode->Launch(ElectrodesDamage);

		CurrentElectrode = Electrode;
		UseEnergy(ElectrodesEnergyCost);
	}
}

void AAllyDrone::ActivateElectrodes()
{
	if(CurrentElectrode != nullptr){
		CurrentElectrode->Activate();
		CurrentElectrode = nullptr;
	}
}

TMap<EResourceTypes, int32> AAllyDrone::GetRequiredMaterials()
{
	switch (DroneState)
	{
	case EAllyDroneState::Broken:
		return DroneDestroyed;
	default:
		return DroneStillAlive;
	}
}

void AAllyDrone::GainEnergy(float Value)
{
	EnergyValue = FMath::Clamp(EnergyValue + Value, 0.f, MaxEnergyValue);
	UpdateEnergyBar();
}

void AAllyDrone::UseEnergy(float Value)
{
	for (auto & Passive : GetUpgradeSlots()) {
		if (Passive) {
			if (Passive->GetUpgrade() == EDroneUpgrade::EnergySaver) {
				Value /= 2;
			}
		}
	}
	EnergyValue = FMath::Clamp(EnergyValue - Value, 0.f, MaxEnergyValue);
	UpdateEnergyBar();
}

void AAllyDrone::DrainEnergy(float Value)
{
	if(Value > 0){
		for (auto& Passive : GetUpgradeSlots()) {
			if (Passive) {
				if (Passive->GetUpgrade() == EDroneUpgrade::EnergyDrain) {
					if (GetEnergyLeft() < MaxEnergyValue) {
						GetHit(DrainDamage);
						GainEnergy(0.5f);
					}
				}
			}
		}
		
	}
}

float AAllyDrone::GetEnergyLeft()
{
	return EnergyValue;
}

// ---------------------------- MOVE FUNCTIONS --------------------------------------
void AAllyDrone::Turn(float Amount)
{
	FRotator ActorRotation = GetActorRotation();

	float Roll = 30.f * FMath::Sign(Amount);
	float RollRotationSpeed = Amount != 0 ? RollSpeed * FMath::Abs(Amount) : RollSpeed;
	float NewRoll = FMath::FInterpTo(ActorRotation.Roll, Roll, GetWorld()->GetDeltaSeconds(), RollRotationSpeed);

	float NewYaw = ActorRotation.Yaw + Amount * YawSpeed;

	CurrentRotation.Roll = NewRoll;
	CurrentRotation.Yaw = NewYaw;
}

void AAllyDrone::Pitch(float Amount)
{
	FRotator ActorRotation = GetActorRotation();

	float NewPitch = ActorRotation.Pitch + Amount * PitchSpeed;
	NewPitch = FMath::Clamp(NewPitch, -MaxPitch, MaxPitch);
	CurrentRotation.Pitch = NewPitch;
}

void AAllyDrone::ForwardThrust(float Amount)
{
	if (Amount != 0) {
		float Increase = CurrentForwardThrust + (Amount * ForwardThrustPower);
		CurrentForwardThrust = FMath::Clamp(Increase, -ForwardOverheat, ForwardOverheat);
	}
	else {
		if (CurrentForwardThrust > 0) {
			float TempSpeed = CurrentForwardThrust - ForwardThrustResistance;
			CurrentForwardThrust = FMath::Clamp(TempSpeed, 0.f, ForwardOverheat);
		}
		else if (CurrentForwardThrust < 0) {
			float TempSpeed = CurrentForwardThrust + ForwardThrustResistance;
			CurrentForwardThrust = FMath::Clamp(TempSpeed, -ForwardOverheat, 0.f);
		}
	}
}

void AAllyDrone::RightThrust(float Amount)
{
	if (Amount != 0) {
		float Increase = CurrentRightThrust + (Amount * RightThrustPower);
		CurrentRightThrust = FMath::Clamp(Increase, -RightOverheat, RightOverheat);
	}
	else {
		if (CurrentRightThrust > 0) {
			float TempSpeed = CurrentRightThrust - RightThrustResistance;
			CurrentRightThrust = FMath::Clamp(TempSpeed, 0.f, RightOverheat);
		}
		else if (CurrentRightThrust < 0) {
			float TempSpeed = CurrentRightThrust + RightThrustResistance;
			CurrentRightThrust = FMath::Clamp(TempSpeed, -RightOverheat, 0.f);
		}
	}
}

void AAllyDrone::UpThrust(float Amount)
{
	if (Amount != 0) {
		float Increase = CurrentUpThrust + (Amount * UpThrustPower);
		CurrentUpThrust = FMath::Clamp(Increase, -UpOverheat, UpOverheat);
	}
	else {
		if (CurrentUpThrust > 0) {
			float TempSpeed = CurrentUpThrust - UpThrustResistance;
			CurrentUpThrust = FMath::Clamp(TempSpeed, 0.f, UpOverheat);
		}
		else if (CurrentUpThrust < 0) {
			float TempSpeed = CurrentUpThrust + UpThrustResistance;
			CurrentUpThrust = FMath::Clamp(TempSpeed, -UpOverheat, 0.f);
		}
	}
}

void AAllyDrone::CameraSwitch()
{
	if (FirstPersonCamera->IsActive()) {
		FirstPersonCamera->Deactivate();
		ThirdPersonCamera->Activate();
	}
	else {
		FirstPersonCamera->Activate();
		ThirdPersonCamera->Deactivate();
	}
}

void AAllyDrone::SpeedUp(float Amount)
{
	if (Amount > 0) ForwardOverheat += ForwardThrustPower;
	else ForwardOverheat -= ForwardThrustResistance;
	
	ForwardOverheat = FMath::Clamp(ForwardOverheat, TempOverheat, ForwardOverheat);
}

void AAllyDrone::Stop()
{
	CurrentForwardThrust = 0;
	CurrentRightThrust = 0;
	CurrentUpThrust = 0;
}


// ---------------------------- INTERACT FUNCTIONS --------------------------------------
void AAllyDrone::InteractCycle()
{
	if (DroneAI->GetAIState() == EAllyDroneIAState::Piloting)
	{
		CurrentlyInteractingActor = GetActorInInteractLine();
		if (GrabbedItem == nullptr) CurrentlyGrabbingActor = GetActorInGrabLine();

		bool bShouldBlank = false;
		if (CurrentlyGrabbingActor != nullptr)
		{
			if (CurrentlyGrabbingActor->IsA(AWeight::StaticClass()))
			{
				InteractWidget->SetInteractText("[G]  Afferra");
				bShouldBlank = false;
			}
			else bShouldBlank = true;
		}

		if (CurrentlyInteractingActor != nullptr)
		{
			if (CurrentlyInteractingActor->IsA(AItems::StaticClass()) || CurrentlyInteractingActor->IsA(ABattery::StaticClass()))
			{
				InteractWidget->SetInteractText("[E]  Raccogli");
				bShouldBlank = false;
			}
			else if (CurrentlyInteractingActor->IsA(AControlPanel::StaticClass()))
			{
				InteractWidget->SetInteractText("[E]  Interagisci");
				bShouldBlank = false;
			}
		}

		if (CurrentlyGrabbingActor == nullptr && CurrentlyInteractingActor == nullptr) bShouldBlank = true;

		if (bShouldBlank) InteractWidget->SetInteractText("");
	}
}


AActor* AAllyDrone::GetActorInInteractLine()
{
	FHitResult Hit;
	FVector StartLine = Drone->GetSocketLocation(FName("CameraSocket"));
	FVector EndLine = StartLine+ GetActorForwardVector() * InteractLineLenght;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(50.f);
	if (GetWorld()->SweepSingleByChannel(Hit, StartLine, EndLine, FRotator::ZeroRotator.Quaternion(), ECollisionChannel::ECC_Visibility, Sphere, CollisionParams)) {
		if (Hit.bBlockingHit) return Hit.GetActor();	
	}
	return nullptr;
}

AActor* AAllyDrone::GetActorInGrabLine()
{
	FHitResult Hit;
	FVector StartLine = GetActorLocation() + GetActorUpVector() * -10.f;
	FVector EndLine = GetActorLocation() + FVector::DownVector * GrabLineLenght;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(10.f);
	if (GetWorld()->SweepSingleByChannel(Hit, StartLine, EndLine, FRotator::ZeroRotator.Quaternion(), ECollisionChannel::ECC_Visibility, Sphere, CollisionParams)) {
		if (Hit.bBlockingHit) return Hit.GetActor();	
	}
	return nullptr;
}

void AAllyDrone::Interact()
{
	if (CurrentlyInteractingActor != nullptr) {
		if (CurrentlyInteractingActor->IsA(ABattery::StaticClass())) {
			ABattery* Battery = Cast<ABattery>(CurrentlyInteractingActor);
			if (Battery) {
				InteractWidget->AddToLog("Recharged");
					OverrideCharge += Battery->Charge;
					Battery->ParticleEffect();
					Battery->Destroy();
					UpdateOverrideBar();
				
			}
		}
		else if (CurrentlyInteractingActor->IsA(AControlPanel::StaticClass())) {
			AControlPanel* ControlPanel = Cast<AControlPanel>(CurrentlyInteractingActor);
			if (ControlPanel) {
				bool Opened = ControlPanel->PanelToggle(OverrideCharge >= OverrideMaxCharge);
				if (Opened) {
					OverrideCharge = 0.f;
					UpdateOverrideBar();
				}
			}
		}
	}
	InteractCycle();
}

void AAllyDrone::Grab()
{
	if (GrabbedItem != nullptr) Detach();
	else Attach();
	InteractCycle();
}

void AAllyDrone::Attach()
{
	if (CurrentlyGrabbingActor != nullptr) {
		if (CurrentlyGrabbingActor->IsA(AWeight::StaticClass())) {
			AWeight* Weight = Cast<AWeight>(CurrentlyGrabbingActor);
			if (Weight) {
				Weight->Mesh->SetSimulatePhysics(false);
				Weight->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				GrabbedItem = Weight;
			}
		}
	}
}

void AAllyDrone::Detach()
{
	if (GrabbedItem != nullptr) {
		GrabbedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedItem->Mesh->SetSimulatePhysics(true);
		GrabbedItem = nullptr;
	}
}

void AAllyDrone::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if(Other->GetInstigator() != this && !Other->IsA(AProjectile::StaticClass())){
		if ((CurrentForwardThrust >= ForwardOverheat) || (CurrentRightThrust >= RightOverheat) || (CurrentUpThrust >= UpOverheat)) GetHit(CollisionDamage);
		Stop();
	}
}

void AAllyDrone::CheckStartFighting()
{
	if (DroneState == EAllyDroneState::Disabled && Archer != nullptr)
	{
		TArray<AEnemy*> Enemies = Archer->GetFightingEnemies();
		if (Enemies.Num() > 0)
		{
			DroneAI->StartFighting(Enemies[0]);
			AnimInstance->SetIsOpen(true);
		}
	}
}

void AAllyDrone::CheckStopFighting()
{
	if (DroneState == EAllyDroneState::Disabled && Archer != nullptr)
	{
		TArray<AEnemy*> Enemies = Archer->GetFightingEnemies();
		if (Enemies.Num() == 0)
		{
			DroneAI->StopFighting();
			AnimInstance->SetIsOpen(false);
		}
		else
		{
			if (DroneAI->GetTarget() == nullptr)
			{
				DroneAI->StartFighting(Enemies[0]);
			}
		}
	}
}

void AAllyDrone::SetOverlappingWorkbench(bool isOverlapping)
{
	bWorkbenchIsOverlapping = isOverlapping;
}

bool AAllyDrone::IsWorkbenchOverlapping()
{
	return bWorkbenchIsOverlapping;
}

bool AAllyDrone::GetIsDown() {
	return DroneAI->GetIsFallen();
}