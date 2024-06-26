#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "SnapCamera.generated.h"

DECLARE_DELEGATE(FOnPhotoSnapSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFocusSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnFocusSignature);

class USceneCaptureComponent2D;
class UCurveFloat;

UCLASS(ClassGroup=Camera, meta=(BlueprintSpawnableComponent))
class BLACKCATJAM_API USnapCamera : public UCameraComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCaptureComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	UCurveFloat* FocusCurve;

	// -- Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	float FocusFOV;
	
	bool Focusing;
	float CurrentTime;
	float CurveValue;
	float InitialFOV;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	bool Focused;
	
public:
	FOnPhotoSnapSignature OnPhotoTaken;
	
	FOnFocusSignature OnCameraFocus;
	FOnUnFocusSignature OnCameraUnFocus;
	
public:	
	// Sets default values for this component's properties
	USnapCamera();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void TakePhoto() const;

	UFUNCTION(BlueprintCallable)
	void FocusCamera();

	float GetNormalisedFOVScale();
	
private:
	void ZoomInCameraUpdate(float DeltaTime);
	void ZoomOutCameraUpdate(float DeltaTime);
};
