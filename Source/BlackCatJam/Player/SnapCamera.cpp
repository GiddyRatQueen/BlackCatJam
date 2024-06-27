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
	FocusViewport = FVector2D(1320, 350);
	CurrentTime = 0.0f;
	CurveValue = 0.0f;
	
	PrimaryComponentTick.bCanEverTick = true;

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture Component"));
	SceneCaptureComponent->SetupAttachment(this);
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SceneCaptureComponent->bCaptureOnMovement = false;
}

// Called when the game starts
void USnapCamera::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();
}

// Called every frame
void USnapCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USnapCamera::TakePhoto() const
{
	OnPhotoTaken.Execute();
	SceneCaptureComponent->CaptureScene();

	// Get List of Cats within View
	TArray<ACat*> cats = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetListOfCats();
	for (ACat* cat : cats)
	{
		// If Cat is on Screen
		if (IsActorWithinFocusRegion(cat))
		{
			OnCatPhotoTaken.Broadcast(cat);
		}
	}
}

void USnapCamera::FocusCamera(EZoomLevel NewZoomLevel)
{
	// Ignore if already at that Zoom Level
	if (ZoomLevel == NewZoomLevel)
		return;

	ZoomedIn = true;
	AdjustCameraZoom(NewZoomLevel);
}

void USnapCamera::FocusCamera(int value)
{
	uint8 zoomValue = (uint8)ZoomLevel;
	uint8 newZoomValue = zoomValue + value;
	newZoomValue = FMath::Clamp(newZoomValue, 0, 2); // Hard Coded Zoom Values (Only Two: Far & Very Far)

	if (newZoomValue > zoomValue)
		OnCameraZoomIn();
	else if (newZoomValue < zoomValue)
		OnCameraZoomOut();
	
	EZoomLevel newZoomLevel = static_cast<EZoomLevel>(newZoomValue);
	if (newZoomLevel != EZoomLevel::Normal)
	{
		FocusCamera(newZoomLevel);
	}
}

bool USnapCamera::IsActorWithinFocusRegion(AActor* Actor) const
{
	FVector2D screenPosition;
	if (PlayerController->ProjectWorldLocationToScreen(Actor->GetActorLocation(), screenPosition))
	{
		FVector2D viewportSize;
		GEngine->GameViewport->GetViewportSize(viewportSize);

		FVector2D center(viewportSize.X / 2, viewportSize.Y / 2);
		FVector2D boxMin = center - FVector2D(FocusViewport.X / 2, FocusViewport.Y / 2);
		FVector2D boxMax = center + FVector2D(FocusViewport.X / 2, FocusViewport.Y / 2);

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
