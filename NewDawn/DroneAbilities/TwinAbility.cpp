#include "TwinAbility.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

UTwinAbility::UTwinAbility()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> ElectrodesTextureObj(TEXT("Texture2D'/Game/HUD/Textures/Electrodes.Electrodes'"));
	if (ElectrodesTextureObj.Object != NULL)
	{
		Texture = ElectrodesTextureObj.Object;
	}
	SetAbilityName(FText::FromString("Twin Electrodes"));
	SetAbilityDescription(FText::FromString("Hold to launch two spheres, release to link them with an electric discharge [30% Energy]"));
	SetDroneAbility(EAllyDroneAbilities::Electrodes);
}