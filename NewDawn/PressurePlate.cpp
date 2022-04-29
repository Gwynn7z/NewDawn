#include "PressurePlate.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Weight.h"

APressurePlate::APressurePlate()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshAsset(TEXT("StaticMesh'/Game/Triggers/PressurePlate/PressurePlate.PressurePlate'"));
	Mesh->SetStaticMesh(BaseMeshAsset.Object);
	Mesh->SetRelativeScale3D(MeshScale);

	Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
	Text->SetupAttachment(RootComponent);

	Text->SetTextRenderColor(Colore);
	Text->SetWorldSize(WordSize);
	Text->SetRelativeLocation(TextLocation);
	Text->SetRelativeRotation(TextRotation);
	Text->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	Text->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

	TBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeightBox"));
	TBox->SetupAttachment(RootComponent);
	TBox->SetRelativeLocation(TBoxRelativeLocation);
	TBox->SetRelativeScale3D(TBoxScale);

	TBox->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::Overlap);
	TBox->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::EndOverlap);

	ItemsToActivate = 1;
	CountingWeights = 0;

	Text->SetText(FText::FromString(FString::FromInt(ItemsToActivate)));
}

void APressurePlate::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Text->SetText(FText::FromString(FString::FromInt(ItemsToActivate)));
}

void APressurePlate::Overlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AWeight::StaticClass())) {
		CountingWeights++;
		Text->SetText(FText::FromString(FString::FromInt(ItemsToActivate - CountingWeights)));
	}
	if (CountingWeights >= ItemsToActivate) ActivateSlider();
}

void APressurePlate::EndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AWeight::StaticClass())) {
		CountingWeights--;
		Text->SetText(FText::FromString(FString::FromInt(ItemsToActivate - CountingWeights)));
	}
	if (CountingWeights < ItemsToActivate) DeactivateSlider();
}