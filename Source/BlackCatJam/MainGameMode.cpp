#include "MainGameMode.h"

#include "Player/SnapCamera.h"

AMainGameMode::AMainGameMode()
{
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (APawn* PlayerPawn = PlayerController->GetPawn())
	{
		PlayerSnapCamera = PlayerPawn->FindComponentByClass<USnapCamera>();
		PlayerSnapCamera->OnPhotoTaken.BindUObject(this, &AMainGameMode::OnPhotoTaken);
		PlayerSnapCamera->OnCameraFocus.AddUniqueDynamic(this, &AMainGameMode::OnCameraFocus);
		PlayerSnapCamera->OnCameraUnFocus.AddUniqueDynamic(this, &AMainGameMode::OnCameraUnFocus);
	}
}

void AMainGameMode::OnPhotoTaken() const
{
	OnPhotoTakenEvent.Broadcast();
}

void AMainGameMode::OnCameraFocus()
{
	OnCameraFocusEvent.Broadcast();
}

void AMainGameMode::OnCameraUnFocus()
{
	OnCameraUnFocusEvent.Broadcast();
}
