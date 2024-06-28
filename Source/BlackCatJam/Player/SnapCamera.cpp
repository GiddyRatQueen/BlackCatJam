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
	
	PrimaryComponentTick.bCanEverTick = true;
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

void USnapCamera::TakePhoto() const
{
	OnPhotoTaken.Broadcast();
	SceneCaptureComponent->CaptureScene();

	// Play Shutter Sound if Avaiable
	if (ShutterSound != nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ShutterSound);
	}
	
	// Get List of Cats within View
	TArray<ACat*> cats = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetListOfCats();
	for (ACat* cat : cats)
	{
		// If Cat is on Screen
		if (IsActorWithinFocusRegion(cat))
		{
			OnCatPhotoTakenEvent.Broadcast(cat);
		}
	}
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

bool USnapCamera::IsActorWithinFocusRegion(AActor* Actor) const
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
