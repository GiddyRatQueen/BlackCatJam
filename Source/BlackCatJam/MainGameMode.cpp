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

void AMainGameMode::RegisterCat(ACat* cat)
{
	Cats.Add(cat);
}

TArray<ACat*> AMainGameMode::GetListOfCats()
{
	return Cats;
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
		PlayerSnapCamera->OnPhotoTaken.AddUniqueDynamic(this, &AMainGameMode::OnPhotoTaken);
		PlayerSnapCamera->OnCatPhotoTakenEvent.AddUniqueDynamic(this, &AMainGameMode::OnCatDetected);
		PlayerSnapCamera->OnCameraZoom.AddUniqueDynamic(this, &AMainGameMode::OnCameraZoom);
	}

	StartTrack();
}

void AMainGameMode::OnPhotoTaken()
{
	OnPhotoTakenEvent.Broadcast();
}

bool AMainGameMode::AreAllCatsPhotographed() const
{
	UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECatType"), true);
	int catTypeCount = enumPtr->NumEnums();
	int photographedCount = PhotographedCats.Num();

	return photographedCount >= catTypeCount;
}

void AMainGameMode::OnCatDetected(ACat* Cat)
{
	ECatType catType = Cat->CatType;
	if (PhotographedCats.IsEmpty())
	{
		OnNewCatPhotograph(catType);
		PhotographedCats.Add(catType);
	}
	else
	{
		for (ECatType Type : PhotographedCats)
		{
			if (catType == Type)
			{
				OnCatPhotograph(catType);
			}
			else
			{
				OnNewCatPhotograph(catType);
				PhotographedCats.Add(catType);
			}
		}
	}

	if (AreAllCatsPhotographed())
	{
		OnMissionComplete();
	}
	
	//UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECatType"), true);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Cat Detected: %s"), *enumPtr->GetDisplayNameTextByValue((int64)Cat->CatType).ToString()));
}

void AMainGameMode::OnPlayerReachedEndOfTrack()
{
	EndTrack();
}

void AMainGameMode::OnCameraZoom(EZoomLevel ZoomLevel)
{
	OnCameraZoomEvent.Broadcast(ZoomLevel);
}
