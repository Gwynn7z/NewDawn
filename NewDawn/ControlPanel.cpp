#include "ControlPanel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AControlPanel::AControlPanel()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshAsset(TEXT("StaticMesh'/Game/Triggers/ControlPanel/Meshes/ControlPanel.ControlPanel'"));
	Mesh->SetStaticMesh(BaseMeshAsset.Object);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);
	Light->SetRelativeLocation(LightLocation);
	Light->SetAttenuationRadius(LightRadius);
	Light->SetIntensity(LightIntensity);

	LockedSpark = CreateDefaultSubobject<UParticleSystem>(TEXT("LockedSpark"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAssetLockedSpark(TEXT("ParticleSystem'/Game/Particles/FXVarietyPack/Particles/P_ky_lightning1.P_ky_lightning1'"));
	if (ParticleAssetLockedSpark.Succeeded()) LockedSpark = ParticleAssetLockedSpark.Object;

	ActiveSpark = CreateDefaultSubobject<UParticleSystem>(TEXT("ActiveSpark"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAssetActiveSpark(TEXT("ParticleSystem'/Game/Particles/FXVarietyPack/Particles/P_ky_shootingStar1.P_ky_shootingStar1'"));
	if (ParticleAssetActiveSpark.Succeeded()) ActiveSpark = ParticleAssetActiveSpark.Object;

	TriggerState = ETriggerState::On;
}

void AControlPanel::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateLight();
}

void AControlPanel::BeginPlay()
{
	Super::BeginPlay();
	UpdateLight();
}

bool AControlPanel::PanelToggle(bool Unlockable)
{
	switch (TriggerState)
	{
	case ETriggerState::On:
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActiveSpark, GetActorLocation() + SparkLocation);
		ToggleSlider();
		break;
	case ETriggerState::Locked:
		if (Unlockable) { //Overcharge
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LockedSpark, GetActorLocation() + SparkLocation);
			TriggerState = ETriggerState::On;
			UpdateLight();
			ToggleSlider();

			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

void AControlPanel::UpdateLight()
{
	switch (TriggerState) {
	case ETriggerState::On:
		Light->SetLightColor(OnColor);
		break;
	case ETriggerState::Off:
		Light->SetLightColor(OffColor);
		break;
	case ETriggerState::Locked:
		Light->SetLightColor(LockedColor);
		break;
	default:
		break;
	}
}