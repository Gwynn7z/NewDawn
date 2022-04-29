#include "ShotAbility.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

UShotAbility::UShotAbility()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> ShotTextureObj(TEXT("Texture2D'/Game/HUD/Textures/ElectricShot.ElectricShot'"));
	if (ShotTextureObj.Object != NULL)
	{
		Texture = ShotTextureObj.Object;
	}
	SetAbilityName(FText::FromString("Electric Shot"));
	SetAbilityDescription(FText::FromString("Shoots an electric projectile   [10%  Energy]"));
	SetDroneAbility(EAllyDroneAbilities::Shot);
}