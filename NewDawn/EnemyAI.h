#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enums.h"
#include "EnemyAI.generated.h"

class UAISenseConfig_Sight;
class AEnemy;
class AArcher;
class AAllyDrone;

UCLASS()
class NEWDAWN_API AEnemyAI : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAI();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Componenti") UAISenseConfig_Sight* ConfigSense;

	void DisableAI(bool bDestroy);
	void Stun(bool State);
	void ResetSensing();

protected:
	UFUNCTION() virtual void SetSightParameters(float VisionAngle = 50.f, float Radius = 1000.f, float LoseSight = 1000.f);
	UFUNCTION() virtual void OnSightUpdate(const TArray<AActor*>& DetectedPawns);
	UFUNCTION() virtual void CheckSensorInAlert() {};
	void SetAlertState(EEnemyIAState AlertState);

	float LostSightTime;
	float TriggeredTime;
	bool ForceDetect;

	AEnemy* EnemyOwner;
	EEnemyIAState IAState;

	AActor* EnemyTarget;
	AArcher* Archer;
	AAllyDrone* Drone;

	FTimerHandle AlertTime;

};
