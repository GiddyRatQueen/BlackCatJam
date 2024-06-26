#include "BlackCatJam/Player/SnapCamera.h"

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
}

void USnapCamera::FocusCamera()
{
	if (Focusing)
	{
		Focused = true;
	}
	
	Focusing = true;
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

		OnCameraFocus.Broadcast();
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

		OnCameraUnFocus.Broadcast();
	}
}
