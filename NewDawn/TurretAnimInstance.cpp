#include "TurretAnimInstance.h"
#include "UObject/ConstructorHelpers.h"

UTurretAnimInstance::UTurretAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ShootM(TEXT("AnimMontage'/Game/Turret/Animations/ShootMontage.ShootMontage'"));
	ShootMontage = ShootM.Object;
}

bool UTurretAnimInstance::IsShooting()
{
	return IsMontagePlaying(ShootMontage);
}

void UTurretAnimInstance::Shoot()
{
	PlayMontage(ShootMontage);
}

void UTurretAnimInstance::LastShoot()
{
	Montage_JumpToSection(FName("EndLoop"), ShootMontage);
}

void UTurretAnimInstance::StopShooting()
{
	StopMontage(ShootMontage, 0.1f);
}
// ---------------------------- ANIMATION FUNCTIONS --------------------------------------
void UTurretAnimInstance::PlayMontage(UAnimMontage* Montage, float PlayRate) {
	if (!Montage_IsPlaying(Montage)) Montage_Play(Montage, PlayRate);
}

void UTurretAnimInstance::MontageJump(UAnimMontage* Montage, FString Section) {
	Montage_JumpToSection(*Section, Montage);
}

bool UTurretAnimInstance::IsMontagePlaying(UAnimMontage* Montage) {
	return Montage_IsPlaying(Montage);
}

void UTurretAnimInstance::StopMontage(UAnimMontage* Montage, float Blend) {
	if (Montage_IsPlaying(Montage)) Montage_Stop(Blend, Montage);
}
