#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ArcherHUD.generated.h"

UCLASS()
class NEWDAWN_API AArcherHUD : public AHUD
{
	GENERATED_BODY()

public:
    AArcherHUD();
    virtual void DrawHUD() override;

protected:
    UPROPERTY(EditDefaultsOnly)  UTexture2D* CrosshairTexture;
};
