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

UTexture2D* AMainGameMode::CreateTextureFromRenderTarget(UTextureRenderTarget2D* RenderTarget) const
{
	FTextureRenderTargetResource* resource = RenderTarget->GameThread_GetRenderTargetResource();
	if (resource)
	{
		TArray<FColor> bitMap;
		bitMap.Empty(RenderTarget->SizeX * RenderTarget->SizeY);

		resource->ReadPixels(bitMap);

		UTexture2D* photograph = UTexture2D::CreateTransient(RenderTarget->SizeX, RenderTarget->SizeY);
		if (photograph)
		{
			void* textureData = photograph->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(textureData, bitMap.GetData(), bitMap.Num() * sizeof(FColor));
			photograph->GetPlatformData()->Mips[0].BulkData.Unlock();
			photograph->UpdateResource();

			return photograph;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, TEXT("ERROR: Failed to Create Texture from Render Target"));
	return nullptr;
}

void AMainGameMode::CreatePhotograph(UTextureRenderTarget2D* RenderTarget, ECatType CatType)
{
	uint8 catTypeValue = (uint8)CatType;
	UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECatType"), true);

	FString textureName = "Photograph - " + enumPtr->GetDisplayNameTextByValue(catTypeValue).ToString();
	UTexture2D* texture = RenderTarget->ConstructTexture2D(this, textureName, EObjectFlags::RF_NoFlags, CTF_DeferCompression);
	if (texture)
	{
		Photographs.Add(texture);
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, "ERROR: Failed to Create Texture From Render Target");
}

void AMainGameMode::OnCatDetected(ACat* Cat, UTextureRenderTarget2D* RenderTarget)
{
	ECatType catType = Cat->CatType;
	if (PhotographedCats.IsEmpty() || !PhotographedCats.Contains(catType))
	{
		OnNewCatPhotograph(catType);
		PhotographedCats.Add(catType);
		
		// Create Texture from Render Target and Save it
		CreatePhotograph(RenderTarget, catType);
	}
	else
	{
		OnCatPhotograph(catType);
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
