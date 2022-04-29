#include "TurretAI.h"
#include "Enemy.h"
#include "Turret.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Archer.h"	
#include "AllyDrone.h"

ATurretAI::ATurretAI() {
	ConfigSense->SightRadius = 1500.f;
	ConfigSense->LoseSightRadius = 1500.f;
	ConfigSense->PeripheralVisionAngleDegrees = 90.f;

	LostSightTime = 2.f;
	TriggeredTime = 1.f;
}

void ATurretAI::BeginPlay()
{
	Super::BeginPlay();

	EnemyOwner = Cast<ATurret>(GetPawn());
}

void ATurretAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector EnemyLocation = EnemyOwner->GetCannonLocation();
	
	switch (EnemyOwner->GetState())
	{
	case EEnemyState::Alert:
		if (EnemyTarget != nullptr) EnemyOwner->SetCannonRotation((EnemyTarget->GetActorLocation() - EnemyLocation).Rotation());
		break;
	case EEnemyState::Fighting:
	{
		FRotator Rotation = (EnemyTarget->GetActorLocation() - EnemyLocation).Rotation();
		EnemyOwner->SetCannonRotation(Rotation);
		break;
	}
	default:
		break;
	}

	if ((IAState == EEnemyIAState::LostSight || IAState == EEnemyIAState::Triggered) && EnemyTarget != nullptr) //Instant activate if close enough	
	{
		if (FVector::Dist(EnemyTarget->GetActorLocation(), EnemyOwner->GetActorLocation()) < ConfigSense->SightRadius / 2)
		{
			GetWorld()->GetTimerManager().ClearTimer(AlertTime);
			CheckSensorInAlert();
		}
	}
}

void ATurretAI::OnSightUpdate(const TArray<AActor*>& DetectedPawns)
{
	Super::OnSightUpdate(DetectedPawns);

	if (Drone == nullptr && Archer == nullptr) {
		EnemyTarget = nullptr;
		if (IAState == EEnemyIAState::Enabled) {
			SetAlertState(EEnemyIAState::LostSight);
			EnemyOwner->StopShooting();
			SetSightParameters(90.f, 1800.f, 2000.f);
		}

	}
	else if (IAState == EEnemyIAState::Disabled) {
		SetAlertState(EEnemyIAState::Triggered);
	}

	EnemyOwner->SetTarget(EnemyTarget);
}

void ATurretAI::CheckSensorInAlert() {
	if (EnemyTarget == nullptr) {
		IAState = EEnemyIAState::Disabled;
		SetSightParameters(90.f, 1500.f, 1500.f);
	}
	else {
		IAState = EEnemyIAState::Enabled;
		EnemyOwner->StartShooting();
		SetSightParameters(90.f, 1500.f, 1500.f);

		if (Archer != nullptr)
		{
			Archer->AddFightingEnemy(EnemyOwner);
		}
		else if (Drone != nullptr)
		{
			Drone->AddFightingEnemy(EnemyOwner);
		}
	}
}
