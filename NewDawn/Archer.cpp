#include "Archer.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraActor.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Bow.h"
#include "NoraBow.h"
#include "CarjaBow.h"
#include "BanukBow.h"
#include "Arrow.h"
#include "Items.h"
#include "Chest.h"
#include "AllyDrone.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprint.h"
#include "ArcherAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "DefensesBars.h"
#include "Enemy.h"
#include "Menu.h"
#include "Bag.h"
#include "WeaponWidget.h"
#include "InteractWidget.h"
#include "Items/Inventory.h"
#include "Items/InventoryItem.h"
#include "Items/BowInventoryItem.h"
#include "Items/ArrowInventoryItem.h"
#include "Items/UsableInventoryItem.h"
#include "Items/ResourcesInventoryItem.h"

AArcher::AArcher()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetCollisionProfileName(FName("OverlapAll"));
	Root->SetCapsuleHalfHeight(100.f);
	Root->SetCapsuleRadius(25.f);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetWorldRotation(FRotator(0.f, 270.f, 0.f));
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkelMesh(TEXT("SkeletalMesh'/Game/Archer/Mesh/Archer.Archer'"));
	Mesh->SetSkeletalMesh(SkelMesh.Object);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	DroneSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DroneArm"));
	DroneSpringArm->SetupAttachment(Mesh);
	DroneSpringArm->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	DroneSpringArm->TargetArmLength = 100.f;
	DroneSpringArm->SetRelativeLocation(FVector(0.f, 0.f, 180.f));

	FloatingDroneLocation = CreateDefaultSubobject<USceneComponent>(TEXT("DroneLocation"));
	FloatingDroneLocation->SetupAttachment(DroneSpringArm);
	FloatingDroneLocation->SetRelativeLocation(FVector(50.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBlueprint(TEXT("AnimBlueprint'/Game/Archer/Animations/ArcherBlueprint.ArcherBlueprint'"));
	if (AnimBlueprint.Object != NULL)
	{
		UAnimBlueprint* AnimBP = AnimBlueprint.Object;
		Mesh->SetAnimInstanceClass(AnimBP->GetAnimBlueprintGeneratedClass());
	}
	
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 300.f;
	CameraSpringArm->bUsePawnControlRotation = false;
	CameraSpringArm->SetRelativeLocation(FVector(0.f, 0.f, 75.f));

	CameraSpringArmBow = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmBow"));
	CameraSpringArmBow->SetupAttachment(RootComponent);
	CameraSpringArmBow->TargetArmLength = 120.f;
	CameraSpringArmBow->bUsePawnControlRotation = false;
	CameraSpringArmBow->SetRelativeLocation(FVector(0.f, 0.f, 75.f));

	static ConstructorHelpers::FObjectFinder<USoundCue> ConcreteRightStepCue(TEXT("SoundCue'/Game/Archer/Sound/FootstepsConcreteR.FootstepsConcreteR'"));
	ConcreteRightStep = ConcreteRightStepCue.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue> ConcreteLeftStepCue(TEXT("SoundCue'/Game/Archer/Sound/FootstepsConcreteL.FootstepsConcreteL'"));
	ConcreteLeftStep = ConcreteLeftStepCue.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue> DirtRightStepCue(TEXT("SoundCue'/Game/Archer/Sound/FootstepsGrassR.FootstepsGrassR'"));
	DirtRightStep = DirtRightStepCue.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue> DirtLeftStepCue(TEXT("SoundCue'/Game/Archer/Sound/FootstepsGrassL.FootstepsGrassL'"));
	DirtLeftStep = DirtLeftStepCue.Object;

	DodgeParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("DodgeParticle"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DodgeParticleAsset(TEXT("ParticleSystem'/Game/Particles/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_FrostGiant/ICE/P_JumpAndDive_Dash.P_JumpAndDive_Dash'"));
	if (DodgeParticleAsset.Succeeded()) DodgeParticle = DodgeParticleAsset.Object;

	UStaticMeshComponent*  QuiverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Quiver"));
	QuiverMesh->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, FName("QuiverSocket"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> QuiverMeshAsset(TEXT("StaticMesh'/Game/Quiver/Quiver.Quiver'"));
	QuiverMesh->SetStaticMesh(QuiverMeshAsset.Object);
	QuiverMesh->SetRelativeScale3D(FVector(0.04f, 0.04f, 0.04f));

	isAiming = false;
	isSprinting = false;
	isFlying = false;
	isGoingToCollide = false;
	isDodging = false;
	isBowBack = true;
	isPilotingDrone = false;

	MaxEquippedBows = 2;
	YawSpeed = 5.f;
	DodgeSpeed = 3.f;
	JumpVy = 0.f;
	MoveIncreaserVx = 0.f;
	MovingDirection = FVector::ZeroVector;

	MoveState = EArcherMovement::Still;
	Speed = EArcherSpeed::Normal;
	LifeState = ELifeState::Alive;
	StandPosition = EArcherStand::Standing;
	

	CurrentlyInteractingActor = nullptr;
	CurrentBow = nullptr;

	float HP = 200.f;
	float TearResistance = -1.f;
	float FireResistance = 50.f;
	float IceResistance = 25.f;
	float ElectResistance = 25.f;

	DefensiveStats = FDefensiveStats(GetDamageMapFromValues(HP, TearResistance, FireResistance, IceResistance, ElectResistance));

	static ConstructorHelpers::FClassFinder<UUserWidget> BarsAsset(TEXT("/Game/HUD/StatBars"));
	BarsAssetClass = BarsAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryAsset(TEXT("/Game/HUD/Menu"));
	MenuAssetClass = InventoryAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InteractAsset(TEXT("/Game/HUD/InteractWidget"));
	InteractAssetClass = InteractAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> WeaponAsset(TEXT("/Game/HUD/WeaponItems"));
	WeaponAssetClass = WeaponAsset.Class;
}

void AArcher::BeginPlay()
{
	Super::BeginPlay();

	Inventory = NewObject<UInventory>();

	//HUD Widgets
	MenuWidget = CreateWidget<UMenu>(GetWorld(), MenuAssetClass);
	InteractWidget = CreateWidget<UInteractWidget>(GetWorld(), InteractAssetClass);
	Bars = CreateWidget<UDefensesBars>(GetWorld(), BarsAssetClass);
	WeaponWidget = CreateWidget<UWeaponWidget>(GetWorld(), WeaponAssetClass);

	WeaponWidget->HideBowWidget(true);
	WeaponWidget->HideUsableWidget(true);

	Inventory->SetOwner(this);

	MenuWidget->SetInventory(Inventory);
	MenuWidget->SetShow(false);

	InteractWidget->AddToViewport();
	WeaponWidget->AddToViewport();
	Bars->AddToViewport();
	SetDefaultBarValues();

	AnimInstance = Cast<UArcherAnimInstance>(Mesh->GetAnimInstance());
	StandUp();
	SetBowBack(true);

	//Spawn Cameras
	FActorSpawnParameters CameraParams;
	CameraParams.Owner = this;
	CameraParams.bNoFail = true;

	Camera = GetWorld()->SpawnActor<ACameraActor>(FVector::ZeroVector, FRotator::ZeroRotator, CameraParams);
	Camera->GetCameraComponent()->bConstrainAspectRatio = false;
	Camera->AttachToComponent(CameraSpringArm, FAttachmentTransformRules::KeepRelativeTransform);

	CameraBow = GetWorld()->SpawnActor<ACameraActor>(FVector::ZeroVector, FRotator::ZeroRotator, CameraParams);
	CameraBow->GetCameraComponent()->bConstrainAspectRatio = false;
	CameraBow->AttachToComponent(CameraSpringArmBow, FAttachmentTransformRules::KeepRelativeTransform);	
	
	CameraBow->SetActorRelativeLocation(FVector(0.f, 70.f, 0.f));

	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(Camera, 0.5f);

	GetWorld()->GetTimerManager().SetTimer(InteractTimer, this, &AArcher::InteractCycle, 0.5f, true);

	Inventory->AddResources(EResourceTypes::CopperFragment, 10);
	Inventory->AddResources(EResourceTypes::MetalPlate, 5);
	Inventory->AddResources(EResourceTypes::Wood, 5);
	Inventory->AddResources(EResourceTypes::ElectronicComponent, 5);
	Inventory->AddResources(EResourceTypes::Wire, 10);
	Inventory->AddArrows(EDamageTypes::Physical, 20);
	Inventory->AddArrows(EDamageTypes::Tear, 5);
	Inventory->AddArrows(EDamageTypes::Fire, 5);
	Inventory->AddArrows(EDamageTypes::Ice, 5);
	Inventory->AddArrows(EDamageTypes::Electric, 5);
}

void AArcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsPawnControlled() && InputEnabled() && LifeState == ELifeState::Alive) {
		if (!isDodging) {
			//Set Movement State
			if (MovingDirection == FVector::ZeroVector) MoveState = EArcherMovement::Still;
			else MoveState = EArcherMovement::Moving;
			if (isFlying) MoveState = EArcherMovement::Flying;

			//Set Speed
			if (isAiming || StandPosition == EArcherStand::Crouched) Speed = EArcherSpeed::Slow;
			else if (isSprinting) Speed = EArcherSpeed::Fast;
			else Speed = EArcherSpeed::Normal;

			//Get Rotations
			FRotator MeshRotation = Mesh->GetRelativeRotation().GetNormalized();
			FRotator CameraRotation = CameraSpringArm->GetRelativeRotation().GetNormalized();
			float NewYawRotation = MeshRotation.Yaw;
			if (isAiming) NewYawRotation = CameraRotation.Yaw - 90.f; //Set Rotation if aiming (always) (90 for default mesh rotation)

			//Get Moving Direction
			FVector MovingVector;
			if (isAiming)  MovingVector = (Mesh->GetRightVector() * MovingDirection.X) + (Mesh->GetForwardVector() * MovingDirection.Y * -1); //If aiming rotate with camera
			else MovingVector = Mesh->GetRightVector(); //If not aiming follow mesh direction

			CollideBlock(MovingVector);
			if (isGoingToCollide) MovingVector = FVector::ZeroVector; //If colliding  stop

			if (MoveState == EArcherMovement::Flying) {

				if (MovingDirection == FVector::ZeroVector) MovingVector = FVector::ZeroVector; //Just only up if not moving

				JumpVy -= 9.81 * DeltaTime;
				MovingVector.Z = JumpVy;

				//Check la capoccia
				FHitResult OutHit;
				FVector Start = CameraSpringArm->GetComponentLocation();
				FVector UpVector = FVector(0.f, 0.f, 1.f);
				FVector End = ((UpVector * 50.f) + Start);
				FCollisionQueryParams CollisionParams;
				if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
					if (OutHit.bBlockingHit && JumpVy > 0) JumpVy = 0.f;
			}
			else if (MoveState == EArcherMovement::Moving) {
				if (!isAiming) NewYawRotation = CameraRotation.Yaw + MovingDirection.Rotation().Yaw - 90.f; //Set Rotation if not aiming (only when walking)

				if (isAiming) MoveIncreaserVx = 1.f; //Start at full speed if aiming
				else {
					YawSpeed = 10.f;
					if (MoveIncreaserVx < 1) {
						if (MoveIncreaserVx == 0.f) {
							YawSpeed = 20.f; //Insta-Rotate if idle to walking
							MoveState = EArcherMovement::Still; //Start animation a bit later
						}
						if (Mesh->GetRelativeRotation().Equals(FRotator(0.f, NewYawRotation, 0.f), 10.f)) {
							MoveIncreaserVx = FMath::Clamp(MoveIncreaserVx + DeltaTime, 0.f, 1.f); //Don't start at full speed if not aiming
						}
					}	
				}
			}
			else if (MoveState == EArcherMovement::Still) MoveIncreaserVx = 0.f;
			
			
			MovingVector.X *= int32(Speed) * MoveIncreaserVx;
			MovingVector.Y *= int32(Speed) * MoveIncreaserVx;
			MovingVector.Z *= int32(EArcherSpeed::Normal);
			AddActorWorldOffset(MovingVector * DeltaTime, true); //Accelerate and move

			//Set Mesh Rotation
			if (MeshRotation.Yaw != NewYawRotation) {
				FRotator NewRotation = FRotator(0.f, NewYawRotation, 0.f);
				if (!isAiming) NewRotation = FMath::RInterpTo(MeshRotation, NewRotation, DeltaTime, YawSpeed); //Don't insta-rotate if not aiming
				Mesh->SetRelativeRotation(NewRotation);
			}
		}

		if (isDodging) {
			CollideBlock(Mesh->GetRightVector());
			FVector DodgeVector = FMath::VInterpTo(GetActorLocation(), DodgeArrive, DeltaTime, DodgeSpeed);
			if(!isGoingToCollide) SetActorLocation(DodgeVector, true);
			if (isGoingToCollide || (FMath::IsNearlyEqual(GetActorLocation().X, DodgeArrive.X, 100.f) && FMath::IsNearlyEqual(GetActorLocation().Y, DodgeArrive.Y, 100.f))) {
				Mesh->SetVisibility(true, true);
				isDodging = false;
				GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(Camera, 1.f);
			}
		}

		//Set Animations
		switch (MoveState)
		{
		case EArcherMovement::Moving:
			if (Speed == EArcherSpeed::Fast) AnimInstance->SetLocomotion(ELocomotion::Running);
			else AnimInstance->SetLocomotion(ELocomotion::Walking);
			break;
		case EArcherMovement::Still:
			AnimInstance->SetLocomotion(ELocomotion::Idle);
			break;
		default:
			break;
		}
	}
	//Gravity
	SimulateWalkGravity();
	AdjustCrouchLocations();
}

void AArcher::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Aim", this, &AArcher::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AArcher::StopAim);

	PlayerInputComponent->BindAxis("Draw", this, &AArcher::Draw);
	PlayerInputComponent->BindAction("Draw", IE_Released, this, &AArcher::Release);

	PlayerInputComponent->BindAction("BowChangeUp", IE_Pressed, this, &AArcher::BowChangeUp);
	PlayerInputComponent->BindAction("BowChangeDown", IE_Pressed, this, &AArcher::BowChangeDown);
	PlayerInputComponent->BindAction("ArrowChangeUp", IE_Pressed, this, &AArcher::ArrowChangeUp);
	PlayerInputComponent->BindAction("ArrowChangeDown", IE_Pressed, this, &AArcher::ArrowChangeDown);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AArcher::StartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AArcher::StopRunning);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AArcher::StartJump);

	PlayerInputComponent->BindAction("SwapController", IE_Pressed, this, &AArcher::SwapToDrone);
	PlayerInputComponent->BindAction("ToggleLight", IE_Pressed, this, &AArcher::ToggleLight);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AArcher::Interact);

	PlayerInputComponent->BindAxis("LookUp", this, &AArcher::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AArcher::Turn);
	PlayerInputComponent->BindAxis("MoveForward", this, &AArcher::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AArcher::MoveRight);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AArcher::Dodge);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AArcher::ToggleCrouch);

	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AArcher::OpenMenu);
	PlayerInputComponent->BindAction("ShortcutItem", IE_Pressed, this, &AArcher::ShortcutItem);

}

void AArcher::AddLog(FString Text)
{
	if (InteractWidget) InteractWidget->AddToLog(Text);
}

void AArcher::InventoryDrop(FUnownedInventory NewInventory)
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ABag::StaticClass());
	if (OverlappingActors.Num() == 0) {
		FActorSpawnParameters Params;
		Params.bNoFail = true;
		Params.Instigator = this;
		Params.Owner = this;
		FVector SpawnLocation = Mesh->GetComponentLocation() + GetActorRightVector() * 100;
		ABag* NewBag = GetWorld()->SpawnActor<ABag>(ABag::StaticClass(), SpawnLocation, FRotator::ZeroRotator, Params);
		NewBag->AddToInventory(NewInventory);
	}
	else {
		ABag* ExistingBag = Cast <ABag>(OverlappingActors[0]);
		ExistingBag->AddToInventory(NewInventory);
	}
}

void AArcher::SetDefaultBarValues()
{
	if (Bars && !isPilotingDrone)
	{
		Bars->SetMainBarMaxValue(DefensiveStats.FullStats[EDamageTypes::Physical]);
		UpdateHPBar();

		Bars->HideSecondaryBar(true);
		Bars->HideEnergyBar(true);
	}
}

void AArcher::UpdateHPBar()
{
	if (Bars && !isPilotingDrone) Bars->SetMainBarValue(DefensiveStats.Stats[EDamageTypes::Physical]);
}

void AArcher::OpenMenu()
{
	MenuWidget->SetShow(true);

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(MenuWidget->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	GetWorld()->GetFirstPlayerController()->SetInputMode(Mode);

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
}

void AArcher::ShortcutItem()
{
	UUsableInventoryItem* Item = WeaponWidget->GetUsableItem();
	if (Item) Item->Use();
}

// ---------------------------- FUNCTIONS --------------------------------------
void AArcher::SwapToDrone() 
{
	if (Drone != nullptr && MoveState != EArcherMovement::Flying && !isDodging) {
		if (Drone->GetLifeState() == EAllyDroneState::Broken) return;
		if(WeaponWidget->IsInViewport()) WeaponWidget->RemoveFromViewport();
		InteractWidget->SetInteractText("");
		isPilotingDrone = true;
		Drone->Pilot();
		SetCapsuleCollision(true); //To Block Drone
		Stop();
		Crouch();
	}
}

void AArcher::Retarget() 
{
	isPilotingDrone = false;
	GetWorld()->GetFirstPlayerController()->Possess(this);
	GetWorld()->GetFirstPlayerController()->SetViewTarget(Camera);
	SetCapsuleCollision(false);
	SetDefaultBarValues();
	if(!WeaponWidget->IsInViewport()) WeaponWidget->AddToViewport();
	if(Drone != nullptr) if(Drone->GetLifeState() != EAllyDroneState::Broken) Drone->CheckStartFighting();
	if (!InputEnabled()) EnableInput(GetWorld()->GetFirstPlayerController());
	MenuWidget->EnableInventory(true);
}

void AArcher::ToggleLight() 
{
	if (Drone != nullptr) if (Drone->GetLifeState() != EAllyDroneState::Broken) Drone->ToggleLight();
}


// ---------------------------- HP FUNCTIONS --------------------------------------
bool AArcher::Heal(float Amount)
{
	bool Result = DefensiveStats.ModifyStat(EDamageTypes::Physical, Amount, EModifierType::Increase);
	if(Result) UpdateHPBar();
	return Result;
}

void AArcher::GetHit(TMap<EDamageTypes, float> IncomingDamage)
{
	DefensiveStats.ModifyStats(IncomingDamage, EModifierType::Decrease);
	for (auto& Damage : DefensiveStats.GetZeroValues()) {
		switch (Damage)
		{
		case EDamageTypes::Physical:
			if (GetWorld()->GetTimerManager().IsTimerActive(BackBowTimer)) GetWorld()->GetTimerManager().ClearTimer(BackBowTimer);
			DisableInput(GetWorld()->GetFirstPlayerController());
			StopAim();
			Stop();

			Camera->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CameraBow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			if (Drone != nullptr)
			{
				Drone->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				Drone->Break();
			}
			LifeState = ELifeState::Dead;
			AnimInstance->SetLifeState(LifeState);
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

void AArcher::AddFightingEnemy(AEnemy* NewEnemy)
{
	if (NewEnemy) {
		FightingEnemies.Add(NewEnemy);
		if (Drone != nullptr) if (Drone->GetLifeState() != EAllyDroneState::Broken) Drone->CheckStartFighting();
	}
}

void AArcher::RemoveFightingEnemy(AEnemy* OldEnemy)
{
	if (FightingEnemies.Contains(OldEnemy)) FightingEnemies.Remove(OldEnemy);
	if (Drone != nullptr) if (Drone->GetLifeState() != EAllyDroneState::Broken) Drone->CheckStopFighting();
}

TArray<AEnemy*> AArcher::GetFightingEnemies()
{
	return FightingEnemies;
}

// ---------------------------- BOW FUNCTIONS --------------------------------------

//----- AIM / STOP AIM

void AArcher::Aim(float Value) 
{
	if (Value > 0 && !isAiming && !isDodging     && CurrentBow != nullptr) 
	{
		GrabBow();
		SetIsAiming(true);

		if (!CurrentBow->HasArrow()) AnimInstance->RechargeBow();

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(CameraBow, 0.2f);
		if (GetWorld()->GetTimerManager().IsTimerActive(BackBowTimer)) GetWorld()->GetTimerManager().ClearTimer(BackBowTimer);
	}
}

void AArcher::StopAim() 
{
	if (CurrentBow != nullptr)
	{
		SetIsDrawing(false);
		AnimInstance->StopDraw();
		SetIsAiming(false);

		CurrentBow->ResetDraw();

		if (!isDodging) GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(Camera, 0.5f);
		GetWorld()->GetTimerManager().SetTimer(BackBowTimer, this, &AArcher::TakeBowBack, 3.0f, false);
	}
}

void AArcher::SetIsAiming(bool IsAiming)
{
	if (CurrentBow != nullptr)
	{
		isAiming = IsAiming;
		AnimInstance->SetIsAiming(IsAiming);
	}
}

//----- AIM / STOP AIM

//---------------------

//----- BOW BACK / GRAB

void AArcher::TakeBowBack()
{
	if (!isBowBack   && CurrentBow != nullptr) {
		SetBowBack(true);
		PutAwayArrow();
		AnimInstance->TakeBackBow();
	}
}

void AArcher::GrabBow()
{
	if (isBowBack) {
		CurrentBow->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, FName("BowUseSocket"));
		CurrentBow->SetActorRelativeLocation(FVector::ZeroVector);
		SetBowBack(false);
	}
}

void AArcher::SetBowBack(bool BowBack)
{
	isBowBack = BowBack;
	AnimInstance->SetIsBowBack(BowBack);
}

void AArcher::PutBowBack() //Notify
{
	if (CurrentBow != nullptr) 
	{
		CurrentBow->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, FName("BackBowSocket"));
		CurrentBow->SetActorRelativeLocation(FVector::ZeroVector);
	}
}

//----- BOW BACK / GRAB

//----------------------

//----- Arrow Knock / Quiver

void AArcher::GrabArrow() //Notify
{
	if (CurrentBow != nullptr) CurrentBow->SetCurrentArrowType(CurrentArrowType);
}

void AArcher::KnockArrow() //Notify
{
	if (Inventory->CheckRemoveArrows(CurrentArrowType) && CurrentBow != nullptr)
	{
		CurrentBow->Recharge();
		WeaponWidget->UpdateStack();
	}
}

void AArcher::PutAwayArrow()
{
	if (CurrentBow != nullptr)
	{
		if (CurrentBow->HasArrow()) {
			Inventory->AddArrows(CurrentArrowType);
			CurrentBow->RemoveArrow();
			WeaponWidget->UpdateStack();
			Inventory->UpdateHUD();
		}
	}
}

bool AArcher::ArrowOutOfQuiver()
{
	if (CurrentBow != nullptr)
	{
		if (CurrentBow->HasArrow()) return true;
		else return false;
	}
	return false;
}

//----- Arrow Knock / Quiver

//----------------------

//----- Draw / Release

void AArcher::Draw(float Value)
{
	if (isAiming && Value > 0.f   &&   CurrentBow != nullptr) {
		if (CurrentBow->HasArrow()) {
			if (!isDrawing) {
				AnimInstance->DrawBow(CurrentBow->GetBowStats().DrawSpeed);
				CurrentBow->Draw();
				SetIsDrawing(true);
			}
		}
		CurrentBow->SetStringLocation(Mesh->GetBoneLocation(FName("middle_02_r")));
	}
}

void AArcher::Release() 
{
	if (isAiming    && CurrentBow != nullptr) {
		if (CurrentBow->GetDrawValue() >= 20) {
			AnimInstance->Release();
			CurrentBow->Release(CameraBow->GetActorLocation() + CameraBow->GetActorForwardVector() - (CameraBow->GetActorRightVector() * 6), CameraBow->GetActorRotation());
		}
		else {
			AnimInstance->StopDraw();
			CurrentBow->ResetDraw();
		}
		
		SetIsDrawing(false);
	}
}

void AArcher::SetIsDrawing(bool IsDrawing)
{
	if (isAiming    && CurrentBow != nullptr)
	{
		isDrawing = IsDrawing;
		AnimInstance->SetIsDrawing(IsDrawing);
	}
}

//----- Draw / Release

//----------------------

//----- Arrow & Bow Change

void AArcher::BowChangeUp() {
	if (!isAiming) {
		if (EquippedBows.Num() > 1 && CurrentBow != nullptr) {
			int32 Index = EquippedBows.IndexOfByKey(CurrentBow);
			if (Index + 1 >= EquippedBows.Num()) Index = 0;
			else ++Index;

			SetBow(EquippedBows[Index]);
		}
	}
}

void AArcher::BowChangeDown() {
	if (!isAiming) {
		if (EquippedBows.Num() > 1 && CurrentBow != nullptr) {
			int32 Index = EquippedBows.IndexOfByKey(CurrentBow);
			if (Index - 1 < 0) Index = EquippedBows.Num() - 1;
			else --Index;

			SetBow(EquippedBows[Index]);
		}
	}
}

void AArcher::SetBow(ABow* NextBow)
{
	SetBowBack(true);

	PutAwayArrow();
	PutBowBack();
	if(CurrentBow != nullptr) CurrentBow->SetActorHiddenInGame(true);

	NextBow->SetActorHiddenInGame(false);
	CurrentBow = NextBow;
	SetArrowType(CurrentBow->GetBowStats().CompatibleArrows[0]);
	GrabBow();

	TakeBowBack();

	for (auto& Bow : Inventory->GetBows()) {
		if(Bow->GetBowStats().Type == CurrentBow->GetBowStats().Type) WeaponWidget->SetWeaponItem(Bow);
	}
	WeaponWidget->HideBowWidget(false);
}

void AArcher::UpdateEquippedBow(UBowInventoryItem* NewBow)
{
	FBowStats NewBowStats = NewBow->GetBowStats();
	
	for (auto Bow : EquippedBows) if (NewBowStats.Type == Bow->GetBowStats().Type) return;

	ABow* Bow = nullptr;
	switch (NewBowStats.Type)
	{
	case EBowTypes::Banuk:
		Bow = GetWorld()->SpawnActor<ABanukBow>(FVector::ZeroVector, FRotator::ZeroRotator);
		break;
	case EBowTypes::Nora:
		Bow = GetWorld()->SpawnActor<ANoraBow>(FVector::ZeroVector, FRotator::ZeroRotator);
		break;
	case EBowTypes::Carja:
		Bow = GetWorld()->SpawnActor<ACarjaBow>(FVector::ZeroVector, FRotator::ZeroRotator);
		break;
	default:
		break;
	}

	if (Bow) {
		Bow->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, FName("BackBowSocket"));
		Bow->SetActorRelativeLocation(FVector::ZeroVector);
		Bow->SetActorEnableCollision(false);
	}

	if (EquippedBows.Num() >= MaxEquippedBows)
	{
		int32 Index = EquippedBows.IndexOfByKey(CurrentBow);
		CurrentBow->Destroy();
		EquippedBows[Index] = Bow;
		SetBow(Bow);
	}
	else {
		EquippedBows.Add(Bow);
		SetBow(Bow);
	}
}

void AArcher::UpdateShortcutItem(UUsableInventoryItem* NewItem)
{
	WeaponWidget->HideUsableWidget(false);
	UUsableInventoryItem* Item = WeaponWidget->GetUsableItem();
	if (Item) Item->SetWeaponWidget(nullptr);
	WeaponWidget->SetShowingUsable(NewItem);
	NewItem->SetWeaponWidget(WeaponWidget);
}

void AArcher::ArrowChangeUp() {
	if (!isAiming && CurrentBow != nullptr) {
		PutAwayArrow();
		TArray<EDamageTypes> DamageTypes = CurrentBow->GetBowStats().CompatibleArrows;
		int32 Index = DamageTypes.IndexOfByKey(CurrentArrowType);
		if (Index + 1 >= DamageTypes.Num()) Index = 0;
		else Index ++;

		SetArrowType(DamageTypes[Index]);
	}
}

void AArcher::ArrowChangeDown() {
	if (!isAiming && CurrentBow != nullptr) {
		PutAwayArrow();
		TArray<EDamageTypes> DamageTypes = CurrentBow->GetBowStats().CompatibleArrows;
		int32 Index = DamageTypes.IndexOfByKey(CurrentArrowType);
		if (Index - 1 < 0) Index = DamageTypes.Num() - 1;
		else Index --;

		SetArrowType(DamageTypes[Index]);
	}
}

void AArcher::SetArrowType(EDamageTypes DamageType)
{
	CurrentArrowType = DamageType;
	for (auto& Arrow : Inventory->GetArrows())
	{
		if (Arrow->GetArrowType() == DamageType)
		{
			if(WeaponWidget->GetArrowItem()) WeaponWidget->GetArrowItem()->SetWeaponWidget(nullptr);
			WeaponWidget->SetArrowItem(Arrow);
			Arrow->SetWeaponWidget(WeaponWidget);
		}
	}
}

//----- Arrow & Bow Change

//------------------------

// ---------------------------- COLLISION FUNCTIONS --------------------------------------

void AArcher::SetCapsuleCollision(bool Collision) {
	if(Collision) Root->SetCollisionProfileName(FName("Pawn"));
	else Root->SetCollisionProfileName(FName("OverlapAll"));
}

AAllyDrone* AArcher::GetOwningDrone()
{
	return Drone;
}


// ---------------------------- MOVE FUNCTIONS --------------------------------------
void AArcher::Turn(float Value) {
	FRotator CameraRotation = CameraSpringArm->GetRelativeRotation();
	CameraRotation.Yaw += Value * GetWorld()->GetDeltaSeconds() * 50.f;
	CameraSpringArm->SetRelativeRotation(CameraRotation);

	FRotator CameraRotationBow = CameraSpringArmBow->GetRelativeRotation();
	CameraRotationBow.Yaw += Value * GetWorld()->GetDeltaSeconds() * 50.f;
	CameraSpringArmBow->SetRelativeRotation(CameraRotationBow);

	if (isAiming) YawSpeed = 5.f;
	else YawSpeed = 10.f;
}

void AArcher::LookUp(float Value) {
	FRotator CameraRotation = CameraSpringArm->GetRelativeRotation();
	CameraRotation.Pitch += Value * GetWorld()->GetDeltaSeconds() * 50.f;
	CameraRotation.Pitch = FMath::Clamp(CameraRotation.Pitch, -50.f, 50.f);
	CameraSpringArm->SetRelativeRotation(CameraRotation);


	FRotator BowCameraRotation = CameraSpringArmBow->GetRelativeRotation();
	BowCameraRotation.Pitch += Value * GetWorld()->GetDeltaSeconds() * 50.f;
	BowCameraRotation.Pitch = FMath::Clamp(BowCameraRotation.Pitch, -50.f, 50.f);
	CameraSpringArmBow->SetRelativeRotation(BowCameraRotation);

	AnimInstance->SetPitchValue(CameraSpringArm->GetRelativeRotation().Pitch * -1);
}

void AArcher::MoveForward(float Value) {
	MovingDirection.X = Value;
}

void AArcher::MoveRight(float Value) {
	MovingDirection.Y = Value;
}

void AArcher::StartRunning() {
	StopAim();
	StandUp();
	isSprinting = true;
}

void AArcher::StopRunning() {
	isSprinting = false;
}

void AArcher::StartJump() {
	if (!isFlying) {
		JumpVy = 2.5f;
		isFlying = true;
		AnimInstance->SetIsJumping(true);
	}
}

void AArcher::SimulateWalkGravity() {
	FHitResult OutHit;
	FVector Start = Mesh->GetSocketLocation(FName("GravityLineTrace"));
	FVector DownVector = FVector(0.f, 0.f, -1.f);
	FVector End = Start + DownVector * GravityLineLength;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility)) {
		if (OutHit.bBlockingHit && JumpVy <= 0) {
			if (isFlying) {
				isFlying = false;
				AnimInstance->SetIsFalling(false);
			}
			JumpVy = 0.f;
			FVector ActorLocation = GetActorLocation();
			ActorLocation.Z = OutHit.ImpactPoint.Z - Mesh->GetRelativeLocation().Z;
			SetActorLocation(ActorLocation);
		}
	}
	else {
		isFlying = true;
		AnimInstance->SetIsFalling(true);
		AnimInstance->SetIsJumping(false);
	};
}

void AArcher::CollideBlock(FVector ForwardDirection)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if(Drone != nullptr) Params.AddIgnoredActor(Drone);

	FCollisionShape Box;
	FVector Center;
	switch (StandPosition)
	{
	case EArcherStand::Standing:
		Box = FCollisionShape::MakeBox(FVector(20.f, 20.f, 60.f));
		Center = GetActorLocation() + FVector(0.f, 0.f, 25.f);
		break;
	case EArcherStand::Crouched:
		Box = FCollisionShape::MakeBox(FVector(20.f, 20.f, 35.f));
		Center = GetActorLocation() -FVector(0.f, 0.f, 5.f);;
		break;
	default:
		break;
	}
	FVector End = Center + ForwardDirection * HitboxExtent;

	if (GetWorld()->SweepSingleByChannel(Hit, Center, End, FRotator::ZeroRotator.Quaternion(), ECollisionChannel::ECC_Visibility, Box, Params)) {
		if (Hit.bBlockingHit) isGoingToCollide = true;
	}
	else isGoingToCollide = false;
}

void AArcher::Stop()
{
	MoveIncreaserVx = 0.f;
	MovingDirection = FVector::ZeroVector;
	MoveState = EArcherMovement::Still;
	Speed = EArcherSpeed::Normal;
	AnimInstance->SetLocomotion(ELocomotion::Idle);
}

void AArcher::LeftStepSound()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DirtLeftStep, Mesh->GetBoneLocation(FName("Foot_L")));
}

void AArcher::RightStepSound()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DirtRightStep, Mesh->GetBoneLocation(FName("Foot_R")));
}

void AArcher::StandUp()
{
	StandPosition = EArcherStand::Standing;
	AnimInstance->SetStand(EArcherStand::Standing);
}

void AArcher::Crouch() 
{
	StopRunning();
	StandPosition = EArcherStand::Crouched;
	AnimInstance->SetStand(EArcherStand::Crouched);
}

void SetNewInterpLocation(USceneComponent* Component, FVector NewLocation, float DeltaTime)
{
	if(Component->GetRelativeLocation().Equals(NewLocation, 1.f)) return;
	FVector NewInterpLocation = FMath::VInterpTo(Component->GetRelativeLocation(), NewLocation, DeltaTime, 2.f); 
	Component->SetRelativeLocation(NewInterpLocation);
}

void AArcher::AdjustCrouchLocations()
{
	switch (StandPosition)
	{
	case EArcherStand::Standing:
		Root->SetCapsuleHalfHeight(100.f);
		SetNewInterpLocation(CameraSpringArm, FVector(0.f, 0.f, 75.f), GetWorld()->GetDeltaSeconds());
		SetNewInterpLocation(CameraSpringArmBow, FVector(0.f, 0.f, 75.f), GetWorld()->GetDeltaSeconds());
		SetNewInterpLocation(DroneSpringArm, FVector(0.f, 0.f, 180.f), GetWorld()->GetDeltaSeconds());
		break;
	case EArcherStand::Crouched:
		Root->SetCapsuleHalfHeight(60.f);
		SetNewInterpLocation(CameraSpringArm, FVector(0.f, 0.f, 45.f), GetWorld()->GetDeltaSeconds());
		SetNewInterpLocation(CameraSpringArmBow, FVector(0.f, 0.f, 45.f), GetWorld()->GetDeltaSeconds());
		SetNewInterpLocation(DroneSpringArm, FVector(0.f, 0.f, 120.f), GetWorld()->GetDeltaSeconds());
		break;
	default:
		break;
	}
}


void AArcher::ToggleCrouch()
{
	switch (StandPosition)
	{
	case EArcherStand::Standing:
		Crouch();
		break;
	case EArcherStand::Crouched:
		StandUp();
		break;
	default:
		break;
	}
}

void AArcher::Dodge()
{
	if (!isFlying && !isDodging) {
		isDodging = true;
		StopAim();
		
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw = CameraSpringArm->GetRelativeRotation().GetNormalized().Yaw + MovingDirection.Rotation().Yaw - 90.f;
		Mesh->SetRelativeRotation(Rotation);

		DodgeArrive = GetActorLocation() + Mesh->GetRightVector() * DodgeDistance;
		Mesh->SetVisibility(false, true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DodgeParticle, DodgeArrive - FVector(0.f, 0.f, 300.f), Mesh->GetRightVector().Rotation());
	}
}


// ---------------------------- INTERACT FUNCTIONS --------------------------------------
AActor* AArcher::GetActorInInteractLine()
{
	FHitResult Hit;
	FVector StartLine;
	FVector EndLine;

	if (isAiming) {
		StartLine = CameraBow->GetActorLocation();
		EndLine = StartLine + CameraBow->GetActorForwardVector() * InteractLineLenght;
	}
	else {
		StartLine = CameraSpringArm->GetComponentLocation();
		EndLine = StartLine + Camera->GetActorForwardVector() * InteractLineLenght;
	}

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	if (Drone != nullptr) if(Drone->IsAttachedTo(this)) CollisionParams.AddIgnoredActor(Drone);
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(50.f);
	if (GetWorld()->SweepSingleByChannel(Hit, StartLine, EndLine, FRotator::ZeroRotator.Quaternion(), ECollisionChannel::ECC_Visibility, Sphere, CollisionParams)) {
		if (Hit.bBlockingHit) return Hit.GetActor();
	}
	return nullptr;
}

void AArcher::InteractCycle()
{
	if (!isPilotingDrone && !isAiming)
	{
		CurrentlyInteractingActor = GetActorInInteractLine();
		if (CurrentlyInteractingActor != nullptr)
		{
			if (CurrentlyInteractingActor->IsA(AItems::StaticClass()) || CurrentlyInteractingActor->IsA(ABow::StaticClass())) InteractWidget->SetInteractText("[E]   Raccogli");
			else if (CurrentlyInteractingActor->IsA(AAllyDrone::StaticClass())) InteractWidget->SetInteractText("[E]   Interagisci");
			else if (CurrentlyInteractingActor->IsA(AChest::StaticClass()))
			{
				AChest* Chest = Cast<AChest>(CurrentlyInteractingActor);
				if(!Chest->GetInventory()->IsEmpty()) InteractWidget->SetInteractText("[E]    Apri");
				else InteractWidget->SetInteractText("");
			}
			else if (CurrentlyInteractingActor->IsA(ABag::StaticClass()))
			{
				ABag* Bag = Cast<ABag>(CurrentlyInteractingActor);
				if (!Bag->GetInventory()->IsEmpty()) InteractWidget->SetInteractText("[E]    Apri");
				else InteractWidget->SetInteractText("");
			}
			else if (CurrentlyInteractingActor->IsA(AEnemy::StaticClass()))
			{
				AEnemy* Enemy = Cast<AEnemy>(CurrentlyInteractingActor);
				if (Enemy->GetState() == EEnemyState::Broken && !Enemy->GetLoot()->IsEmpty()) InteractWidget->SetInteractText("[E]   Raccogli");
				else InteractWidget->SetInteractText("");
			}
			else InteractWidget->SetInteractText("");
		}
		else InteractWidget->SetInteractText("");
		
	}
}

void AArcher::Interact()
{
	if (CurrentlyInteractingActor != nullptr) {
		if (CurrentlyInteractingActor->IsA(AItems::StaticClass())) {
			AItems* Item = Cast<AItems>(CurrentlyInteractingActor);
			if (Item) {
				int32 Exceeded = Inventory->AddUsables(Item->GetUsableItem().Key, Item->GetUsableItem().Value);
				if(Exceeded == 0) {
					AddLog("(" + FString::FromInt(Item->GetUsableItem().Value) + ") " + UsableName[Item->GetUsableItem().Key]);
					Item->Destroy();
				}
				else if(Exceeded != Item->GetUsableItem().Value){
					Item->RemoveFromStack(Item->GetUsableItem().Value - Exceeded);
					AddLog("(" + FString::FromInt(Item->GetUsableItem().Value - Exceeded) + ") " + UsableName[Item->GetUsableItem().Key]);
				}
				else {
					AddLog("You can't gather this item");
				}
				WeaponWidget->UpdateItemStack();
			}
		}
		else if (CurrentlyInteractingActor->IsA(ABow::StaticClass())) {
			ABow* Bow = Cast<ABow>(CurrentlyInteractingActor);
			bool Added = Inventory->AddBow(Bow->GetBowStats());
			if (Added) {
				AddLog("New Bow: " + BowName[Bow->GetBowStats().Type]);
				Bow->Destroy();
			}
			else {
				AddLog("You can't gather this bow");
			}
		}
		else if (CurrentlyInteractingActor->IsA(AAllyDrone::StaticClass())) {
			AAllyDrone* NewDrone = Cast<AAllyDrone>(CurrentlyInteractingActor);
			if (Drone == nullptr)
			{
				Drone = NewDrone;
				Drone->SetArcher(this);		
				DisableInput(GetWorld()->GetFirstPlayerController());
				Stop();
				Drone->SwapToArcher();
				Drone->SetHUDBarWidget(Bars);
				Drone->SetHUDInteractWidget(InteractWidget);
				Drone->ShareMenu(MenuWidget);
				MenuWidget->SetDrone(Drone);

			}
			else if (Drone == NewDrone && Drone->GetLifeState() == EAllyDroneState::Broken)
			{
				DisableInput(GetWorld()->GetFirstPlayerController());
				Stop();
				Drone->SwapToArcher();
			}
		}
		else if (CurrentlyInteractingActor->IsA(AChest::StaticClass())) {
			AChest* Chest = Cast<AChest>(CurrentlyInteractingActor);
			AddToInventory(Chest->GetInventory());
		}
		else if (CurrentlyInteractingActor->IsA(ABag::StaticClass())) {
			ABag* Bag = Cast<ABag>(CurrentlyInteractingActor);
			AddToInventory(Bag->GetInventory());
		}
		else if (CurrentlyInteractingActor->IsA(AEnemy::StaticClass())) {
			AEnemy* Enemy = Cast<AEnemy>(CurrentlyInteractingActor);
			if (Enemy->GetState() == EEnemyState::Broken) {
				AddToInventory(Enemy->GetLoot());
			}
		}
		InteractCycle();
	}
}

void AArcher::AddToInventory(FUnownedInventory* NewInventory)
{
	if (!NewInventory->IsEmpty())
	{
		TMap<EBowTypes, FBowStats> Weapons = NewInventory->GetWeapons();
		TMap<EResourceTypes, int32> Resources = NewInventory->GetResources();
		TMap<EUsableTypes, int32> Usables = NewInventory->GetUsables();
		TMap<EDamageTypes, int32> Ammunitions = NewInventory->GetAmmunitions();
		int32 x = 0;
		for (auto& Resource : Resources) {
			int32 Exceed = Inventory->AddResources(Resource.Key, Resource.Value);
			NewInventory->UpdateResource(Resource.Key, Resource.Value - Exceed, EModifierType::Decrease);
			AddLog("(" + FString::FromInt(Resource.Value - Exceed) + ") " + ResourceName[Resource.Key]);
			x++;
		}

		for (auto& Usable : Usables) {
			int32 Exceed = Inventory->AddUsables(Usable.Key, Usable.Value);
			NewInventory->UpdateUsable(Usable.Key, Usable.Value - Exceed, EModifierType::Decrease);
			AddLog("(" + FString::FromInt(Usable.Value - Exceed) + ") " + UsableName[Usable.Key]);
			x++;
		}

		for (auto& Ammunition : Ammunitions) {
			int32 Exceed = Inventory->AddArrows(Ammunition.Key, Ammunition.Value);
			NewInventory->UpdateAmmunition(Ammunition.Key, Ammunition.Value - Exceed, EModifierType::Decrease);
			AddLog("(" + FString::FromInt(Ammunition.Value - Exceed) + ") " + ArrowName[Ammunition.Key]);
			x++;
		}
	}
	WeaponWidget->UpdateStack();
	WeaponWidget->UpdateItemStack();
}