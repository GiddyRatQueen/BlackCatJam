#include "MainGameMode.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "Player/SnapCamera.h"

AMainGameMode::AMainGameMode()
{
}

void AMainGameMode::StartTrack()
{
	TrackStarted = true;
	TrackEnded = false;
	IsMovingAlongTrack = true;
	
	PlayerCharacter->StartMovingAlongTrack();
	OnTrackStart.Broadcast();
}

void AMainGameMode::EndTrack()
{
	TrackStarted = false;
	TrackEnded = true;
	IsMovingAlongTrack = false;
	
	OnTrackEnd.Broadcast();
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (APawn* PlayerPawn = PlayerController->GetPawn())
	{
		PlayerCharacter = Cast<APlayerCharacter>(PlayerPawn);
		PlayerCharacter->OnReachedEndOfTrack.AddUniqueDynamic(this, &AMainGameMode::OnPlayerReachedEndOfTrack);
		
		PlayerSnapCamera = PlayerPawn->FindComponentByClass<USnapCamera>();
		PlayerSnapCamera->OnPhotoTaken.BindUObject(this, &AMainGameMode::OnPhotoTaken);
		PlayerSnapCamera->OnCameraFocus.AddUniqueDynamic(this, &AMainGameMode::OnCameraFocus);
		PlayerSnapCamera->OnCameraUnFocus.AddUniqueDynamic(this, &AMainGameMode::OnCameraUnFocus);
	}

	StartTrack();
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

void AMainGameMode::OnPlayerReachedEndOfTrack()
{
	EndTrack();
}
