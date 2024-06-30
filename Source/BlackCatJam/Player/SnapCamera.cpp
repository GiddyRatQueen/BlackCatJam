#include "BlackCatJam/Player/SnapCamera.h"

#include "BlackCatJam/Cat.h"
#include "BlackCatJam/MainGameMode.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
USnapCamera::USnapCamera()
{
	bUsePawnControlRotation = true;
	InitialFOV = FieldOfView;
	CurrentFOV = InitialFOV;
	CameraViewport = FVector2D(1320, 350);
	CurrentTime = 0.0f;
	CurveValue = 0.0f;
	CanTakePhotograph = true;
	
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void USnapCamera::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();
	SceneCaptureComponent = PlayerController->GetPawn()->FindComponentByClass<USceneCaptureComponent2D>();
}

// Called every frame
void USnapCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USnapCamera::TakePhoto()
{
	if (!CanTakePhotograph)
		return;
	
	OnPhotoTaken.Broadcast();
	SceneCaptureComponent->CaptureScene();
	
	// Get List of Cats within View
	TArray<ACat*> cats = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetListOfCats();
	for (ACat* cat : cats)
	{
		// Is the Cat within a Certain Range?
		if (IsActorWithinRange(cat))
		{
			// Is the Cat on Screen?
			if (IsActorWithinFocusRegion(cat))
			{
				// Is the Cat Obstructed by Something?
				if (!IsActorObstructed(cat))
				{
					OnCatPhotoTakenEvent.Broadcast(cat, SceneCaptureComponent->TextureTarget);
				}
			}
		}
	}

	// Play Shutter Sound if Available
	if (ShutterSound != nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ShutterSound);
	}

	StartPhotographDelayTimer();
}

void USnapCamera::FocusCamera(EZoomLevel NewZoomLevel)
{
	// Ignore if already at that Zoom Level
	if (ZoomLevel == NewZoomLevel)
		return;
	
	OnCameraZoom.Broadcast(NewZoomLevel);
	AdjustCameraZoom(NewZoomLevel);
}

void USnapCamera::FocusCamera(int value)
{
	if (FocusCurve == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("ERROR: No Focus Curve set for Snap Camera / Player Camera"));
		return;
	}
	
	uint8 zoomValue = (uint8)ZoomLevel;
	uint8 newZoomValue = zoomValue + value;
	newZoomValue = FMath::Clamp(newZoomValue, 0, 2); // Hard Coded Zoom Values (Only Two: Far & Very Far)
	
	EZoomLevel newZoomLevel = static_cast<EZoomLevel>(newZoomValue);
	if (newZoomLevel != EZoomLevel::Normal)
	{
		FocusCamera(newZoomLevel);
	}
}

void USnapCamera::ResetCamera()
{
	ZoomedIn = false;
	IsAdjustingZoom = false;
	GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);

	OnCameraZoom.Broadcast(EZoomLevel::Normal);
	AdjustCameraZoom(EZoomLevel::Normal);
}

float USnapCamera::GetNormalisedFOVScale()
{
	float factor = (FieldOfView - InitialFOV) / (CurrentFOV - InitialFOV);
	return FMath::Clamp(factor, 0.0f, 1.0f);
}

void USnapCamera::AdjustCameraZoom(EZoomLevel NewZoomLevel)
{
	IsAdjustingZoom = true;
	CurveValue = 0.0f;
	CurrentTime = 0.0f;
	CurrentFOV = FieldOfView;
	
	GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, [this, NewZoomLevel]()
	{
		CurrentTime += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		CurveValue = FocusCurve->GetFloatValue(CurrentTime);
		
		float FOV = GetFOVLevel(NewZoomLevel);
		float newFOV = FMath::Lerp(CurrentFOV, FOV, CurveValue);
		SetFieldOfView(newFOV);
		SceneCaptureComponent->FOVAngle = newFOV;

		if (CurveValue >= 1.0f)
		{
			IsAdjustingZoom = false;
			ZoomLevel = NewZoomLevel;
			CurrentFOV = FOV;
			CurveValue = 0.0f;
			CurrentTime = 0.0f;

			SetFieldOfView(FOV);
			GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
		}
		
	}, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), true);
}

float USnapCamera::GetFOVLevel(EZoomLevel NewZoomLevel)
{
	switch (NewZoomLevel)
	{
	case EZoomLevel::Normal:
		return InitialFOV;

	case EZoomLevel::Far:
		return FarFOV;

	case EZoomLevel::VeryFar:
		return VeryFarFOV;
	}

	return InitialFOV;
}

bool USnapCamera::IsActorWithinFocusRegion(const AActor* Actor) const
{
	FVector2D screenPosition;
	if (PlayerController->ProjectWorldLocationToScreen(Actor->GetActorLocation(), screenPosition))
	{
		int viewportSizeX;
		int viewportSizeY;
		PlayerController->GetViewportSize(viewportSizeX, viewportSizeY);
		FVector2D viewportSize = FVector2D(viewportSizeX, viewportSizeY);

		FVector2D center(viewportSize.X / 2, viewportSize.Y / 2);
		FVector2D boxMin = center - FVector2D(CameraViewport.X / 2, CameraViewport.Y / 2);
		FVector2D boxMax = center + FVector2D(CameraViewport.X / 2, CameraViewport.Y / 2);

		return screenPosition.X >= boxMin.X && screenPosition.X <= boxMax.X &&
			screenPosition.Y >= boxMin.Y && screenPosition.Y <= boxMax.Y;
	}

	return false;
}

bool USnapCamera::IsActorWithinRange(const AActor* Actor) const
{
	if (DetectionLength <= 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, TEXT("ERROR: Detection Length for Snap Camera is 0"));
		return false;
	}

	float distance = (Actor->GetActorLocation() - GetOwner()->GetActorLocation()).Length();
	distance += GetRangeBasedOnFOV();
	if (distance <= DetectionLength)
	{
		return true;
	}

	return false;
}

bool USnapCamera::IsActorObstructed(const AActor* Actor) const
{
	FVector start = GetOwner()->GetActorLocation();
	FVector end = Actor->GetActorLocation();
	FHitResult hit;

	if (GetWorld())
	{
		bool actorHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, FCollisionQueryParams(), FCollisionResponseParams());
		//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.0f, 0, 4.0f);
		if (actorHit && hit.GetActor())
		{
			ACat* cat = Cast<ACat>(hit.GetActor());
			if (cat)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, hit.GetActor()->GetFName().ToString());
				return false;
			}
		}

		return true;
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, TEXT("ERROR: Can't Get World for Raycast"));
	return true;
}

float USnapCamera::GetRangeBasedOnFOV() const
{
	return CurrentFOV * 10;
}

void USnapCamera::StartPhotographDelayTimer()
{
	PhotographTimer = PhotographDelay;
	CanTakePhotograph = false;
	
	GetWorld()->GetTimerManager().SetTimer(PhotographTimerHandle, [this]()
	{
		PhotographTimer -= UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		if (PhotographTimer <= 0.0f)
		{
			CanTakePhotograph = true;
			PhotographTimer = 0.0f;
			
			GetWorld()->GetTimerManager().ClearTimer(PhotographTimerHandle);
		}
		
	}, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), true);
}

FVector2D USnapCamera::CalculateViewportBasedOnFOV()
{
	float fovInRadians = FMath::DegreesToRadians(SceneCaptureComponent->FOVAngle);
	float viewportHeight = 2 * 1000.0f * FMath::Tan(fovInRadians / 2);
	float viewportWidth = viewportHeight * AspectRatio;

	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);
	float pixelsPerUnitX = viewportSize.X / viewportWidth;
	float pixelsPerUnitY = viewportSize.Y / viewportHeight;

	return FVector2D(pixelsPerUnitX * viewportWidth, pixelsPerUnitY * viewportHeight);
}
