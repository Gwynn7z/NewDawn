#include "AllyDroneAnimInstance.h"
#include "UObject/ConstructorHelpers.h"

UAllyDroneAnimInstance::UAllyDroneAnimInstance() 
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ShootMontageAsset(TEXT("AnimMontage'/Game/Drone/Animations/ShootMontage.ShootMontage'"));
	ShootMontage = ShootMontageAsset.Object;
}


void UAllyDroneAnimInstance::Shoot()
{
	PlayMontage(ShootMontage);
}

// ---------------------------- ANIMATION FUNCTIONS --------------------------------------
void UAllyDroneAnimInstance::PlayMontage(UAnimMontage* Montage, float PlayRate) {
	if (!Montage_IsPlaying(Montage)) Montage_Play(Montage, PlayRate);
}
