#include "Bow.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprint.h"
#include "Sound/SoundCue.h"
#include "Engine/Engine.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Arrow.h"
#include "Archer.h"
#include "BowAnimInstance.h"

ABow::ABow()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(FName("BlockAll"));

	static ConstructorHelpers::FObjectFinder<USoundCue> StringCue(TEXT("SoundCue'/Game/Bows/Sound/BowDrawCue.BowDrawCue'"));
	StringPullSound = StringCue.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> StringReleaseCue(TEXT("SoundCue'/Game/Bows/Sound/BowReleaseCue.BowReleaseCue'"));
	StringReleaseSound = StringReleaseCue.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> KnockArrowCue(TEXT("SoundCue'/Game/Bows/Sound/ArrowKnockCue.ArrowKnockCue'"));
	KnockArrowSound = KnockArrowCue.Object;

	StringPullAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StringAudioComp"));
	StringPullAudioComponent->bAutoActivate = false;
	StringPullAudioComponent->SetupAttachment(RootComponent);
	
	DrawValue = 0.f;
	RootBoneName = "root";
}

void ABow::BeginPlay() 
{
	Super::BeginPlay();

	AnimInstance = Cast<UBowAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetBraceHeight(BowInfos.BraceHeight);

	Arrow = nullptr;
	if (StringPullAudioComponent->IsValidLowLevelFast()) StringPullAudioComponent->SetSound(StringPullSound);

}

void ABow::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	if (IsDrawing) {
		DrawValue += DeltaTime * BowInfos.DrawSpeed;
		SetDrawValue(FMath::Clamp(DrawValue, 0.f, 100.f));

		if (DrawValue >= 20 && !IsStringBending)
		{
			StringPullAudioComponent->Play();
			IsStringBending = true;
		}

		FVector ScaledLocation;
		FRotator ScaledRotation;
		Mesh->TransformToBoneSpace(FName(RootBoneName), StringLocation, FRotator::ZeroRotator, ScaledLocation, ScaledRotation);
		AnimInstance->SetTransformLocation(ScaledLocation);
	}
}

void ABow::Draw() 
{
	if (!IsDrawing)	SetIsDrawing(true);
}

void ABow::Release(FVector Location, FRotator Rotation) 
{
	SetIsDrawing(false);
	IsStringBending = false;

	if (HasArrow()) {
		StringPullAudioComponent->Stop();
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), StringReleaseSound, GetActorLocation());
		Arrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Arrow->SetReleaseInfo(Location, Rotation);
		Arrow->SetDamage(BowInfos.DamageStats.Stats, BowInfos.DrawStrength, DrawValue);
		Arrow->Release();
		Arrow = nullptr;
		SetDrawValue(0.f);
	}
}

void ABow::ResetDraw() 
{
	StringPullAudioComponent->Stop();

	SetIsDrawing(false);
	IsStringBending = false;
	SetDrawValue(0.f);
}

void ABow::Recharge() 
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.bNoFail = true;
	
	Params.Instigator = Cast<AArcher>(GetAttachParentActor());
	Arrow = GetWorld()->SpawnActor<AArrow>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
	if (Arrow) {
		Arrow->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, FName("ArrowSocket"));
		Arrow->SetActorScale3D(FVector(1.f, 1.f, 1.f));
		Arrow->SetArrowType(CurrentArrowType);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), KnockArrowSound, GetActorLocation());
	}
}

void ABow::RemoveArrow() 
{
	if (HasArrow()) {
		Arrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), KnockArrowSound, GetActorLocation());
		Arrow->Destroy();
		Arrow = nullptr;
	}
}

void ABow::SetCurrentArrowType(EDamageTypes ArrowType)
{
	CurrentArrowType = ArrowType;
}

void ABow::SetDrawValue(float Value) 
{
	DrawValue = Value;
}

void ABow::SetIsDrawing(bool Drawing)
{
	IsDrawing = Drawing;
	AnimInstance->SetIsDrawing(Drawing);
}