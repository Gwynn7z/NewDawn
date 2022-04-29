#include "FloatingDroneIA.h"
#include "Enemy.h"
#include "FloatingDrone.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Waypoint.h"
#include "Archer.h"
#include "AllyDrone.h"

AFloatingDroneIA::AFloatingDroneIA() {
	ConfigSense->SightRadius = 1500.f;
	ConfigSense->LoseSightRadius = 2500.f;
	ConfigSense->PeripheralVisionAngleDegrees = 60.f;

	LostSightTime = 3.f;
	TriggeredTime = 3.f;
}

void AFloatingDroneIA::BeginPlay()
{
	Super::BeginPlay();

	EnemyOwner = Cast<AFloatingDrone>(GetPawn());

	//Get Owned Waypoints
	TArray<AActor*> TempWaypoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypoint::StaticClass(), TempWaypoints);
	for (int32 i = 0; i < TempWaypoints.Num(); i++) {
		AWaypoint* WayPoint = Cast<AWaypoint>(TempWaypoints[i]);
		if (WayPoint->IsOwner(EnemyOwner)) {
			Steps.Add(WayPoint);
		}
	}
}

void AFloatingDroneIA::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Steps.Num() != 0) {
		FVector EnemyLocation = EnemyOwner->GetActorLocation();
		FRotator EnemyRotation = EnemyOwner->GetActorRotation();

		switch (EnemyOwner->GetState()) {
		case EEnemyState::Idle:
		{
			if (Positions.Num() == 0) ReturnToPosition = false;
			if (GetWorld()->GetTimerManager().IsTimerActive(TimerPosition))	GetWorld()->GetTimerManager().ClearTimer(TimerPosition);

			if (ReturnToPosition) { //Instant return to position if no obstacles within
				FHitResult Hit;
				FVector StartLocation = EnemyLocation;
				FVector EndLocation = Steps[StepNum + StepIncrease]->GetActorLocation();
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(EnemyOwner);
				if (!GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, Params)) {
					ReturnToPosition = false;
					Positions.Empty();
				}
			}

			FVector NextStepLocation;
			if (!ReturnToPosition) {
				if (StepNum + StepIncrease >= Steps.Num() || StepNum + StepIncrease < 0) StepIncrease *= -1;
				NextStepLocation = Steps[StepNum + StepIncrease]->GetActorLocation();
			}
			else NextStepLocation = Positions[Positions.Num() - 1];

			FRotator Rotation;
			switch (PatrolStates) {
			case 0:
			{
				Rotation = (NextStepLocation - EnemyLocation).Rotation();
				EnemyOwner->SetActorRotation(FMath::RInterpTo(EnemyRotation, Rotation, DeltaTime, EnemyOwner->PatrolRotationSpeed));
				if (EnemyRotation.Equals(Rotation, 2.f)) {
					NextPatrolStep();
					EnemyOwner->SetActorRotation(Rotation);
					TemporaryRotation = Rotation;
				}

				break;
			}
			case 1:
			{
				EnemyOwner->AddActorWorldOffset(EnemyOwner->GetActorForwardVector() * EnemyOwner->Speed);
				if (EnemyLocation.Equals(NextStepLocation, 30.f)) {
					if (ReturnToPosition) {
						Positions.RemoveAt(Positions.Num() - 1);
						PatrolStates = 0;
					}
					else {
						StepNum += StepIncrease;
						NextPatrolStep();
					}
				}

				break;
			}
			case 2:
			{
				if (EnemyRotation.Equals(TemporaryRotation + FRotator(0.f, 90.f, 0.f), 10.f)) RotationWay = -1;
				else if (EnemyRotation.Equals(TemporaryRotation + FRotator(0.f, -90.f, 0.f), 10.f)) RotationWay = 1;
				Rotation = EnemyRotation;
				Rotation.Yaw = TemporaryRotation.Yaw + (90.f * RotationWay);
				EnemyOwner->SetActorRotation(FMath::RInterpTo(EnemyRotation, Rotation, DeltaTime, 1.f));
				if (!GetWorld()->GetTimerManager().IsTimerActive(TimerLookAround)) GetWorld()->GetTimerManager().SetTimer(TimerLookAround, this, &AFloatingDroneIA::NextPatrolStep, 5.f, false);

				break;
			}
			default:
				PatrolStates = 0;
				break;
			}
			break;
		}
		case EEnemyState::Fighting:
		{
			if(!GetWorld()->GetTimerManager().IsTimerActive(TimerDodge)) GetWorld()->GetTimerManager().SetTimer(TimerDodge, this, &AFloatingDroneIA::Dodge, FMath::FRandRange(1.f, 2.5f));
			ReturnToPosition = true;
			PatrolStates = 0;

			if (!GetWorld()->GetTimerManager().IsTimerActive(TimerPosition)) {
				Positions.Empty();
				GetWorld()->GetTimerManager().SetTimer(TimerPosition, this, &AFloatingDroneIA::PlaceReturnWaypoint, 1.0f, true);
			}

			FRotator Rotation = (EnemyTarget->GetActorLocation() - EnemyLocation).Rotation();
			EnemyOwner->SetActorRotation(FMath::RInterpTo(EnemyRotation, Rotation, DeltaTime, EnemyOwner->InFightRotationSpeed));

			if (isDodging) {
				FVector Loc = FMath::VInterpTo(EnemyLocation, DodgeLocation, DeltaTime, 5.f);
				EnemyOwner->SetActorLocation(Loc, true);
				if (EnemyLocation.Equals(DodgeLocation, 50.f)) isDodging = false;
			}
			if (FVector::Dist(EnemyTarget->GetActorLocation(), EnemyLocation) > MaxFightDistance) {
				EnemyOwner->AddActorWorldOffset(EnemyOwner->GetActorForwardVector() * EnemyOwner->Speed, true);
			}
			else if (FVector::Dist(EnemyTarget->GetActorLocation(), EnemyLocation) < MinFightDistance) {
				EnemyOwner->AddActorWorldOffset(EnemyOwner->GetActorForwardVector() * EnemyOwner->Speed * -1, true);
			}
			break;
		}
		case EEnemyState::Alert:
			PatrolStates = 0;
			isDodging = false;
			if (EnemyTarget != nullptr) 
				EnemyOwner->SetActorRotation(FMath::RInterpTo(EnemyRotation, (EnemyTarget->GetActorLocation() - EnemyLocation).Rotation(), DeltaTime, EnemyOwner->InFightRotationSpeed));
			else if(AlertRotation != FRotator::ZeroRotator)
				EnemyOwner->SetActorRotation(FMath::RInterpTo(EnemyRotation, AlertRotation, DeltaTime, EnemyOwner->InFightRotationSpeed));
			break;
		case EEnemyState::Broken:
			if (GetWorld()->GetTimerManager().IsTimerActive(TimerPosition))	GetWorld()->GetTimerManager().ClearTimer(TimerPosition);
			if (GetWorld()->GetTimerManager().IsTimerActive(TimerDodge)) GetWorld()->GetTimerManager().ClearTimer(TimerDodge);
			break;
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
	else IAState = EEnemyIAState::Off;
}

void AFloatingDroneIA::OnSightUpdate(const TArray<AActor*>& DetectedPawns)
{
	Super::OnSightUpdate(DetectedPawns);

	if (Drone == nullptr && Archer == nullptr) {
		EnemyTarget = nullptr;
		if (IAState == EEnemyIAState::Enabled) {
			SetAlertState(EEnemyIAState::LostSight);
			SetSightParameters(90.f, 1500.f, 2500.f);
		}
	}
	else if (IAState == EEnemyIAState::Disabled) {
		SetAlertState(EEnemyIAState::Triggered);
	}
	EnemyOwner->SetTarget(EnemyTarget);
}

void AFloatingDroneIA::CheckSensorInAlert() {
	if (EnemyTarget == nullptr) {
		IAState = EEnemyIAState::Disabled;
		EnemyOwner->ExitCombatMode();
		SetSightParameters(60.f, 1500.f, 2500.f);
	}
	else {
		IAState = EEnemyIAState::Enabled;
		EnemyOwner->EnterCombatMode();
		SetSightParameters(180.f, 2000.f, 2500);

		if (Archer != nullptr)
		{
			Archer->AddFightingEnemy(EnemyOwner);
		}
		else if (Drone != nullptr)
		{
			Drone->AddFightingEnemy(EnemyOwner);
		}
	}
	AlertRotation = FRotator::ZeroRotator;
}


void AFloatingDroneIA::HitNotify(AActor* NotifierActor)
{
	if (IAState != EEnemyIAState::Off) {
		AlertRotation = (NotifierActor->GetActorLocation() - EnemyOwner->GetActorLocation()).Rotation();
		if (!GetWorld()->GetTimerManager().IsTimerActive(AlertTime)) GetWorld()->GetTimerManager().SetTimer(AlertTime, this, &AFloatingDroneIA::CheckSensorInAlert, TriggeredTime, false);
		IAState = EEnemyIAState::Triggered;
		SetSightParameters(80.f, 3000.f, 3500.f);
	}
}

FVector AFloatingDroneIA::FindDodgeLocation(bool Direction)
{
	FVector EnemyTargetLocation = EnemyTarget->GetActorLocation();
	FVector EnemyLocation = EnemyOwner->GetActorLocation();

	FRotator Rotation = (EnemyLocation - EnemyTargetLocation).Rotation();
	Rotation.Normalize();

	float RandomYaw = Direction ? FMath::FRandRange(30.f, 90.f) : FMath::FRandRange(-30.f, -90.f);
	FRotator NewRotation = Rotation + FRotator(0.f, RandomYaw, 0.f);

	FVector NewLocation = EnemyTargetLocation + NewRotation.Vector() * MaxFightDistance;
	NewLocation.Z = EnemyLocation.Z;
	return NewLocation;
}

void AFloatingDroneIA::Dodge()
{
	if (EnemyTarget == nullptr) return;

	bool RandomDirection = FMath::RandBool();
	
	FVector Start = EnemyOwner->GetActorLocation();
	FVector End = FindDodgeLocation(RandomDirection);
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(EnemyOwner);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, Params)) End = FindDodgeLocation(!RandomDirection);

	DodgeLocation = End;
	isDodging = true;
}

void AFloatingDroneIA::PlaceReturnWaypoint()
{
	Positions.Add(EnemyOwner->GetActorLocation());
}

void AFloatingDroneIA::NextPatrolStep()
{
	PatrolStates++;
}