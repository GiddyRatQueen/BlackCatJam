#include "BlackCatJam/Player/SnapCamera.h"

#include "BlackCatJam/Cat.h"
#include "BlackCatJam/MainGameMode.h"
#include "Components/SceneCaptureComponent2D.h"

// Sets default values for this component's properties
USnapCamera::USnapCamera()
{
	bUsePawnControlRotation = true;
	InitialFOV = FieldOfView;
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

	if (Focusing && !Focused)
	{
		ZoomInCameraUpdate(DeltaTime);
	}
	else if (Focusing && Focused)
	{
		ZoomOutCameraUpdate(DeltaTime);
	}
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

void USnapCamera::FocusCamera()
{
	if (Focusing)
	{
		Focused = true;
	}

	if (!Focused)
	{
		OnCameraFocus.Broadcast();
	}
	else
	{
		OnCameraUnFocus.Broadcast();
	}
	
	Focusing = true;
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
	float factor = (FieldOfView - InitialFOV) / (FocusFOV - InitialFOV);
	return FMath::Clamp(factor, 0.0f, 1.0f);
}

void USnapCamera::ZoomInCameraUpdate(float DeltaTime)
{
	CurrentTime += DeltaTime;
	CurveValue = FocusCurve->GetFloatValue(CurrentTime);
		
	float newFOV = FMath::Lerp(InitialFOV, FocusFOV, CurveValue);
	SetFieldOfView(newFOV);
	SceneCaptureComponent->FOVAngle = newFOV;

	if (CurveValue >= 1.0f)
	{
		Focused = true;
		Focusing = false;

		SetFieldOfView(FocusFOV);
		CurveValue = 1.0f;
	}
}

void USnapCamera::ZoomOutCameraUpdate(float DeltaTime)
{
	CurrentTime -= DeltaTime;
	CurveValue = FocusCurve->GetFloatValue(CurrentTime);
		
	float newFOV = FMath::Lerp(InitialFOV, FocusFOV, CurveValue);
	SetFieldOfView(newFOV);
	SceneCaptureComponent->FOVAngle = newFOV;

	if (CurveValue <= 0.01f)
	{
		Focused = false;
		Focusing = false;
		
		SetFieldOfView(InitialFOV);
		CurrentTime = 0.0f;
		CurveValue = 0.0f;
	}
}
