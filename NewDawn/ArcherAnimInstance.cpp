#include "ArcherAnimInstance.h"
#include "UObject/ConstructorHelpers.h"

UArcherAnimInstance::UArcherAnimInstance() 
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> TakeBackBowM(TEXT("AnimMontage'/Game/Archer/Animations/TakeBackBowAnimations/TakeBackBowMontage.TakeBackBowMontage'"));
	TakeBackBowMontage = TakeBackBowM.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DrawMontageM(TEXT("AnimMontage'/Game/Archer/Animations/DrawAnimations/DrawMontage.DrawMontage'"));
	DrawMontage = DrawMontageM.Object;
}

void UArcherAnimInstance::DrawBow(float BowSpeed)
{
	PlayMontage(DrawMontage, BowSpeed /100);
	MontageJump(DrawMontage, "Start");
}

void UArcherAnimInstance::RechargeBow()
{
	PlayMontage(DrawMontage);
	MontageJump(DrawMontage, "Grab");
}

void UArcherAnimInstance::TakeBackBow()
{
	PlayMontage(TakeBackBowMontage);
}

void UArcherAnimInstance::Release()
{
	PlayMontage(DrawMontage);
	MontageJump(DrawMontage, "Release");
}

void UArcherAnimInstance::StopDraw()
{
	StopMontage(DrawMontage, 0.2f);
}

// ---------------------------- ANIMATION FUNCTIONS --------------------------------------
void UArcherAnimInstance::PlayMontage(UAnimMontage* Montage, float PlayRate) {
	if (!Montage_IsPlaying(Montage)) Montage_Play(Montage, PlayRate);
}

void UArcherAnimInstance::StopMontage(UAnimMontage* Montage, float Blend) {
	if (Montage_IsPlaying(Montage)) Montage_Stop(Blend, Montage);
}

void UArcherAnimInstance::MontageJump(UAnimMontage* Montage, FString Section) {
	Montage_JumpToSection(*Section, Montage);
}

FName UArcherAnimInstance::GetMontageSection(UAnimMontage* Montage) {
	return Montage_GetCurrentSection(Montage);
}

bool UArcherAnimInstance::IsMontagePlaying(UAnimMontage* Montage) {
	return Montage_IsPlaying(Montage);
}

