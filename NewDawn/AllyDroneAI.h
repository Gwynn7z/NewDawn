#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enums.h"
#include "Structs.h"
#include "AllyDroneAI.generated.h"

class AAllyDrone;
class AEnemy;
class AArcher;

UCLASS()
class NEWDAWN_API AAllyDroneAI : public AAIController
{
	GENERATED_BODY()

public:
	AAllyDroneAI();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	void DisableAI();
	void GotoIdle();
	bool GetIsFallen();

	AEnemy* GetTarget();

	void StartFighting(AEnemy* NewTarget);
	void StopFighting();

	FORCEINLINE EAllyDroneIAState GetAIState() { return IAState; };
	FORCEINLINE void SetIAState(EAllyDroneIAState NewState) { IAState = NewState; };

	void BreakDrone();
	void SetArcher(AArcher* NewArcher);
	void ReviveDrone();

private:
	void SetTarget(AEnemy* NewTarget);

	FTimerHandle ShootTimer;
	float FireRateMax;
	float FireRateMin;

	bool canLand;
	bool brokenAfterReturn;
	bool isFallen;

	EAllyDroneIAState IAState;
	AAllyDrone* DroneOwner;
	AArcher* Archer;
	AEnemy* Target;
};
