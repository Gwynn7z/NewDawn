#include "EnemyAI.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Enemy.h"
#include "AllyDrone.h"
#include "Archer.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

AEnemyAI::AEnemyAI() {
	PrimaryActorTick.bCanEverTick = true;

	ConfigSense = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("PawnSight"));

	UAIPerceptionComponent* AIComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PawnSensor"));
	SetPerceptionComponent(*AIComp);

	ConfigSense->DetectionByAffiliation.bDetectEnemies = true;
	ConfigSense->DetectionByAffiliation.bDetectNeutrals = true;
	ConfigSense->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->bEditableWhenInherited = true;
	GetPerceptionComponent()->SetDominantSense(ConfigSense->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*ConfigSense);

	IAState = EEnemyIAState::Disabled;
}

void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();

	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AEnemyAI::OnSightUpdate);

	EnemyOwner = Cast<AEnemy>(GetPawn());
}

void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (IAState)
	{
	case EEnemyIAState::Disabled:
		EnemyOwner->SetState(EEnemyState::Idle);
		break;
	case EEnemyIAState::Enabled:
		EnemyOwner->SetState(EEnemyState::Fighting);
		break;
	case EEnemyIAState::Triggered:
	case EEnemyIAState::LostSight:
		EnemyOwner->SetState(EEnemyState::Alert);	
		break;
	default:
		break;
	}
}

void AEnemyAI::OnSightUpdate(const TArray<AActor*>& DetectedPawns)
{
	if (IAState == EEnemyIAState::Off) return;
	
    //Detected Pawns in Range
    for (int32 i = 0; i < DetectedPawns.Num(); i++) {
        AActor* DetectedPawn = DetectedPawns[i];
        if (DetectedPawn->IsA(AArcher::StaticClass()) || DetectedPawn->IsA(AAllyDrone::StaticClass())) {
            FActorPerceptionBlueprintInfo Info;
            bool HasPercepted = PerceptionComponent->GetActorsPerception(DetectedPawn, Info);
            if (HasPercepted) {
                TArray<FAIStimulus> Stimulus = Info.LastSensedStimuli;
                //Senses that detected the pawn
                for (int32 j = 0; j < Stimulus.Num(); j++) {
                    if (Stimulus[j].Type == ConfigSense->GetSenseID()) { //Sight Sense
                        bool SenseResult = Stimulus[j].WasSuccessfullySensed();
                        if (SenseResult) { //Enter Sight
                            if (DetectedPawn->IsA(AArcher::StaticClass())) Archer = Cast<AArcher>(DetectedPawn);
                            else if (DetectedPawn->IsA(AAllyDrone::StaticClass())) Drone = Cast<AAllyDrone>(DetectedPawn);
                        }
                        else { //Left Sight
							if (DetectedPawn->IsA(AArcher::StaticClass()))
							{
								if (Archer != nullptr)
								{
									Archer->RemoveFightingEnemy(EnemyOwner);
								}
								Archer = nullptr;
							}
							else if (DetectedPawn->IsA(AAllyDrone::StaticClass()))
							{
								if (Drone != nullptr)
								{
									Drone->RemoveFightingEnemy(EnemyOwner);
								}
								Drone = nullptr;
							}
                        }
                    }
                }
            }
        }
    }
	
	if (Drone != nullptr) {
		if (Drone->GetLifeState() == EAllyDroneState::Active) EnemyTarget = Drone;
		else Drone = nullptr;
	}
	if (Archer != nullptr) {
		if (Archer->GetLifeState() == ELifeState::Alive) EnemyTarget = Archer;
		else Archer = nullptr;
	}
}

void AEnemyAI::Stun(bool State)
{
	if (State) DisableAI(false);
	else {
		if (!GetPerceptionComponent()->OnPerceptionUpdated.IsBound()) GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AEnemyAI::OnSightUpdate);
		SetAlertState(EEnemyIAState::LostSight);
	}
}

void AEnemyAI::DisableAI(bool bDestroy) {
	if (Archer != nullptr) Archer->RemoveFightingEnemy(EnemyOwner);
	if (Drone != nullptr) Drone->RemoveFightingEnemy(EnemyOwner);
	Archer = nullptr;
	Drone = nullptr;
	
	IAState = EEnemyIAState::Off;
	if(bDestroy) EnemyOwner->SetState(EEnemyState::Broken, true);
	else EnemyOwner->SetState(EEnemyState::Off, true);

	if(GetPerceptionComponent()->OnPerceptionUpdated.IsBound()) GetPerceptionComponent()->OnPerceptionUpdated.Clear();
	if (GetWorld()->GetTimerManager().IsTimerActive(AlertTime))	GetWorld()->GetTimerManager().ClearTimer(AlertTime);
}

void AEnemyAI::SetAlertState(EEnemyIAState AlertState)
{
	switch (AlertState)
	{
	case EEnemyIAState::Triggered:
		GetWorld()->GetTimerManager().SetTimer(AlertTime, this, &AEnemyAI::CheckSensorInAlert, TriggeredTime, false);
		IAState = EEnemyIAState::Triggered;
		break;
	case EEnemyIAState::LostSight:
		GetWorld()->GetTimerManager().SetTimer(AlertTime, this, &AEnemyAI::CheckSensorInAlert, LostSightTime, false);
		IAState = EEnemyIAState::LostSight;
		break;
	default:
		break;
	}
}

void AEnemyAI::SetSightParameters(float VisionAngle, float Radius, float LoseSight)
{
	ConfigSense->SightRadius = Radius;
	ConfigSense->LoseSightRadius = LoseSight;
	ConfigSense->PeripheralVisionAngleDegrees = VisionAngle;

	PerceptionComponent->RequestStimuliListenerUpdate();
}


void AEnemyAI::ResetSensing()
{
	SetSightParameters(1.f, 1.f, 1.f);
}