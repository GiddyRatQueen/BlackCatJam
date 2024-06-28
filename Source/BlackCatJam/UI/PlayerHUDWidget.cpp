#include "BlackCatJam/UI/PlayerHUDWidget.h"

#include "BlackCatJam/MainGameMode.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AMainGameMode* gameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		gameMode->OnCameraZoomEvent.AddUniqueDynamic(this, &UPlayerHUDWidget::CameraZoomCallback);
		gameMode->OnPhotoTakenEvent.AddUniqueDynamic(this, &UPlayerHUDWidget::OnPhotoTaken);
	}

	SetCameraViewport();
}

void UPlayerHUDWidget::CameraZoomCallback(EZoomLevel ZoomLevel)
{
	OnCameraZoom(ZoomLevel);

	if (ZoomLevel == EZoomLevel::Normal)
	{
		IsCameraFocusing = false;
	}
	else
	{
		IsCameraFocusing = true;
	}
}
