#include "Waypoint.h"
#include "Components/SceneComponent.h"

AWaypoint::AWaypoint()
{
	PrimaryActorTick.bCanEverTick = false;

	WayPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WayPoint"));
	SetRootComponent(WayPoint);
}