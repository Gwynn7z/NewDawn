#include "ArcherHUD.h"
#include "CanvasItem.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Archer.h"
#include "Engine/Canvas.h"

AArcherHUD::AArcherHUD() {
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/Bows/crosshair.crosshair'"));

    if (CrosshairTexObj.Object != NULL)
    {
        CrosshairTexture = CrosshairTexObj.Object;
    }
}

void AArcherHUD::DrawHUD()
{
    Super::DrawHUD();

    if (CrosshairTexture)
    {
        FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
        FVector2D CrossHairDrawPosition(Center.X - (CrosshairTexture->GetSurfaceWidth() * 0.5f), Center.Y - (CrosshairTexture->GetSurfaceHeight() * 0.5f));
        FCanvasTileItem TileItem(CrossHairDrawPosition, CrosshairTexture->Resource, FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
        if (Pawn) {
            AArcher* Archer = Cast<AArcher>(Pawn);
            if (Archer) if (Archer->GetIsAiming()) Canvas->DrawItem(TileItem);
        }
    }
}