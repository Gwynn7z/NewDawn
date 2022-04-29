#include "AllyDroneAI.h"
#include "Camera/CameraActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "AllyDrone.h"
#include "Archer.h"
#include "Enemy.h"
#include "TimerManager.h"

AAllyDroneAI::AAllyDroneAI() {
	PrimaryActorTick.bCanEverTick = true;

	IAState = EAllyDroneIAState::Idle;

	FireRateMin = 1.f;
	FireRateMax = 2.f;
}

void AAllyDroneAI::BeginPlay()
{
	Super::BeginPlay();
}

void AAllyDroneAI::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	DroneOwner = Cast<AAllyDrone>(GetPawn());
}

void AAllyDroneAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (IAState) {
	case EAllyDroneIAState::Idle:
	case EAllyDroneIAState::Fighting:
	case EAllyDroneIAState::ReturningToIdle:
	case EAllyDroneIAState::Off:
		DroneOwner->SetDroneState(EAllyDroneState::Disabled);
		break;
	case EAllyDroneIAState::Piloting:
		DroneOwner->SetDroneState(EAllyDroneState::Active);
		break;
	case EAllyDroneIAState::Broken:
		DroneOwner->SetDroneState(EAllyDroneState::Broken);
		break;
	}


	switch (IAState)
	{
	case EAllyDroneIAState::ReturningToIdle:
	{
		if (Archer != nullptr)
		{
			DroneOwner->SetActorLocation(FMath::VInterpTo(DroneOwner->GetActorLocation(), Archer->FloatingDroneLocation->GetComponentLocation(), DeltaTime, 2.f));
			if (DroneOwner->GetActorLocation().Equals(Archer->FloatingDroneLocation->GetComponentLocation(), 20.f)) {
				DroneOwner->AttachToComponent(Archer->FloatingDroneLocation, FAttachmentTransformRules::KeepWorldTransform);
				DroneOwner->CurrentRotation = FRotator::ZeroRotator;

				if (!brokenAfterReturn) {
					IAState = EAllyDroneIAState::Idle;
				}
				else
				{
					IAState = EAllyDroneIAState::Broken;
					DroneOwner->SetDroneState(EAllyDroneState::Broken);
				}
				brokenAfterReturn = false;
				DroneOwner->EnableInput(GetWorld()->GetFirstPlayerController());

				Archer->Retarget();
			}
			break;
		}
	}
	case EAllyDroneIAState::Idle:
	{
		DroneOwner->AddActorLocalOffset(FVector(0.f, 0.f, (FMath::Sin(GetWorld()->GetTimeSeconds() + DeltaTime * 5) - FMath::Sin(GetWorld()->GetTimeSeconds()))));
		if (Archer != nullptr) DroneOwner->CurrentRotation.Yaw = Archer->Camera->GetActorRotation().Yaw;
		else DroneOwner->CurrentRotation = FRotator::ZeroRotator;
		FRotator NewRotation = FMath::RInterpTo(DroneOwner->GetActorRotation(), DroneOwner->CurrentRotation, DeltaTime, 5.f);
		DroneOwner->SetActorRotation(NewRotation);
		break;
	}
	case EAllyDroneIAState::Fighting:
	{
		if (Target != nullptr)
		{
			FRotator Rotation = (Target->GetActorLocation() - DroneOwner->GetActorLocation()).Rotation();
			FRotator NewRotation = FMath::RInterpTo(DroneOwner->GetActorRotation(), Rotation, DeltaTime, 8.f);
			DroneOwner->CurrentRotation.Yaw = NewRotation.Yaw;
			DroneOwner->SetActorRotation(NewRotation);
		}
		break;
	}
	case EAllyDroneIAState::Broken:
	{
		if (canLand)
		{
			if(Archer != nullptr) DroneOwner->MoveIgnoreActorAdd(Archer);

			FHitResult Hit;
			DroneOwner->AddActorLocalOffset(FVector(0.f, 0.f, -1.f), true, &Hit);
			if (Hit.bBlockingHit) {
				canLand = false;
				isFallen = true;
			}
			DroneOwner->CurrentRotation = FRotator::ZeroRotator;
			FRotator NewRotation = FMath::RInterpTo(DroneOwner->GetActorRotation(), DroneOwner->CurrentRotation, DeltaTime, 5.f);
			DroneOwner->SetActorRotation(NewRotation);	
		}
		else if (Archer != nullptr && !isFallen) {
			if (Archer != nullptr) DroneOwner->CurrentRotation.Yaw = Archer->Camera->GetActorRotation().Yaw;
			else DroneOwner->CurrentRotation = FRotator::ZeroRotator;
			FRotator NewRotation = FMath::RInterpTo(DroneOwner->GetActorRotation(), DroneOwner->CurrentRotation, DeltaTime, 5.f);
			DroneOwner->SetActorRotation(NewRotation);
		}
		break;
	}
	}
}

void AAllyDroneAI::DisableAI() {
	StopFighting();
	IAState = EAllyDroneIAState::Piloting;
}

void AAllyDroneAI::GotoIdle() {
	if (IAState == EAllyDroneIAState::Broken) {
		brokenAfterReturn = true;
		isFallen = false;
	}
	DroneOwner->DisableInput(GetWorld()->GetFirstPlayerController());
	IAState = EAllyDroneIAState::ReturningToIdle;
}

void AAllyDroneAI::BreakDrone() {
	canLand = true;
	StopFighting();
	if(IAState == EAllyDroneIAState::Piloting && Archer != nullptr) Archer->Retarget();
	IAState = EAllyDroneIAState::Broken;
}

void AAllyDroneAI::SetArcher(AArcher* NewArcher) {
	Archer = NewArcher;
	IAState = EAllyDroneIAState::Idle;
}

void AAllyDroneAI::SetTarget(AEnemy* NewTarget) {
	Target = NewTarget;
}

AEnemy* AAllyDroneAI::GetTarget()
{
	return Target;
}

void AAllyDroneAI::StartFighting(AEnemy* NewTarget)
{
	SetIAState(EAllyDroneIAState::Fighting);
	SetTarget(NewTarget);

	float RandomNumber = FMath::FRandRange(FireRateMin, FireRateMax);
	if (!GetWorld()->GetTimerManager().IsTimerActive(ShootTimer)) GetWorld()->GetTimerManager().SetTimer(ShootTimer, DroneOwner, &AAllyDrone::Shoot, RandomNumber, true);
}

void AAllyDroneAI::StopFighting()
{
	if (IAState != EAllyDroneIAState::Piloting) SetIAState(EAllyDroneIAState::Idle);
	SetTarget(nullptr);

	if (GetWorld()->GetTimerManager().IsTimerActive(ShootTimer)) GetWorld()->GetTimerManager().ClearTimer(ShootTimer);
}

void AAllyDroneAI::ReviveDrone()
{
	IAState = EAllyDroneIAState::Idle;
}

bool AAllyDroneAI::GetIsFallen()
{
	return isFallen || canLand;
}