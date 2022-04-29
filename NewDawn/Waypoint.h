#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Waypoint.generated.h"

class USceneComponent;
class AFloatingDrone;

UCLASS()
class NEWDAWN_API AWaypoint : public AActor
{
	GENERATED_BODY()

public:
	AWaypoint();
	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* WayPoint;
	UPROPERTY(EditAnywhere, Category = "Variabili") AFloatingDrone* EnemyOwner;

	FORCEINLINE UFUNCTION() bool IsOwner(AFloatingDrone* Drone) { return (EnemyOwner == Drone); };
};
