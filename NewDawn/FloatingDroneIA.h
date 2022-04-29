#pragma once

#include "CoreMinimal.h"
#include "EnemyAI.h"
#include "FloatingDroneIA.generated.h"

class AWaypoint;
class AFloatingDrone;

UCLASS()
class NEWDAWN_API AFloatingDroneIA : public AEnemyAI
{
	GENERATED_BODY()

public:
	AFloatingDroneIA();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void HitNotify(AActor* NotifierActor);

protected:
	virtual void OnSightUpdate(const TArray<AActor*>& DetectedPawns) override;
	virtual void CheckSensorInAlert() override;

private:
	FVector FindDodgeLocation(bool Direction);

	AFloatingDrone* EnemyOwner;

	UFUNCTION() void PlaceReturnWaypoint();
	void NextPatrolStep();
	UFUNCTION() void Dodge();

	TArray<AWaypoint*> Steps;
	TArray<FVector> Positions;

	FTimerHandle TimerPosition;
	FTimerHandle TimerLookAround;
	FTimerHandle TimerDodge;

	int32 StepNum = 0;
	int32 StepIncrease = 1;
	int32 PatrolStates = 0;

	FVector DodgeLocation;
	FRotator TemporaryRotation;
	int32 RotationWay = 1;

	const float MaxFightDistance = 500.f;
	const float MinFightDistance = 400.f;
	bool ReturnToPosition = false;
	bool isDodging = false;
	FRotator AlertRotation;
};
