#include "InteractWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "TimerManager.h"

void UInteractWidget::SetInteractText(FString Text)
{
	Interact->SetText(FText::FromString(Text));
}

void UInteractWidget::AddToLog(FString Text)
{
	FTimerHandle NewHandle;
	GetWorld()->GetTimerManager().SetTimer(NewHandle, this, &UInteractWidget::RemoveFromLog, 2.f, false);

	UTextBlock* NewText = NewObject<UTextBlock>(UTextBlock::StaticClass());
	
	NewText->SetText(FText::FromString(Text));
	NewText->Font.Size = 20;
	NewText->Font.TypefaceFontName = FName("Italic");
	NewText->Font.OutlineSettings.OutlineSize = 3;
	NewText->SetJustification(ETextJustify::Left);
	Log->AddChildToVerticalBox(NewText);
}

void UInteractWidget::RemoveFromLog()
{
	if(Log->GetAllChildren().Num() > 0) Log->RemoveChildAt(0);
}
