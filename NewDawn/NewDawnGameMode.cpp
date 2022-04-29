#include "NewDawnGameMode.h"
#include "ArcherHUD.h"
#include "Archer.h"
#include "AllyDrone.h"


ANewDawnGameMode::ANewDawnGameMode()
{
	DefaultPawnClass = AArcher::StaticClass();
	HUDClass = AArcherHUD::StaticClass();
}