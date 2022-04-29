#pragma once

#include "CoreMinimal.h"
#include "EnemyAI.h"
#include "TurretAI.generated.h"

class ATurret;

UCLASS()
class NEWDAWN_API ATurretAI : public AEnemyAI
{
	GENERATED_BODY()

public:
	ATurretAI();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnSightUpdate(const TArray<AActor*>& DetectedPawns) override;
	virtual void CheckSensorInAlert() override;

private:
	ATurret* EnemyOwner;
};
