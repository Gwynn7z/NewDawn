#include "FloatingDroneAnimInstance.h"
#include "UObject/ConstructorHelpers.h"

UFloatingDroneAnimInstance::UFloatingDroneAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ShootDXMontageM(TEXT("AnimMontage'/Game/FloatingDrone/Animations/FDShootDX_Montage.FDShootDX_Montage'"));
	ShootDXMontage = ShootDXMontageM.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ShootSXMontageM(TEXT("AnimMontage'/Game/FloatingDrone/Animations/FDShootSX_Montage.FDShootSX_Montage'"));
	ShootSXMontage = ShootSXMontageM.Object;
}

void UFloatingDroneAnimInstance::ShootDX()
{
	PlayMontage(ShootDXMontage);
}

void UFloatingDroneAnimInstance::ShootSX()
{
	PlayMontage(ShootSXMontage);
}


// ---------------------------- ANIMATION FUNCTIONS --------------------------------------
void UFloatingDroneAnimInstance::PlayMontage(UAnimMontage* Montage, float PlayRate) {
	if (!Montage_IsPlaying(Montage)) Montage_Play(Montage, PlayRate);
}