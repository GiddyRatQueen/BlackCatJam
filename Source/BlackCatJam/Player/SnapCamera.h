#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "SnapCamera.generated.h"

UENUM(BlueprintType)
enum class EZoomLevel : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Far UMETA(DisplayName = "Far"),
	VeryFar UMETA(DisplayName = "Very Far"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraZoomSignature, EZoomLevel, ZoomLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoTakenSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCatPhotoTakenSignature, class ACat*, Cat);

class USceneCaptureComponent2D;
class UCurveFloat;
class USoundCue;

UCLASS(ClassGroup=Camera, meta=(BlueprintSpawnableComponent))
class BLACKCATJAM_API USnapCamera : public UCameraComponent
{
	GENERATED_BODY()

	// -- Components
	USceneCaptureComponent2D* SceneCaptureComponent;

	// -- Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	UCurveFloat* FocusCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	float FarFOV;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	float VeryFarFOV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	FVector2D FocusViewport;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio, meta=(AllowPrivateAccess = "true"))
	USoundBase* ShutterSound;

	FTimerHandle ZoomTimerHandle;
	bool IsAdjustingZoom;
	float InitialFOV;
	float CurrentFOV;
	
	float CurrentTime;
	float CurveValue;
	
	APlayerController* PlayerController;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	bool ZoomedIn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraFocus, meta=(AllowPrivateAccess = "true"))
	EZoomLevel ZoomLevel;

	UPROPERTY(BlueprintAssignable)
	FOnCameraZoomSignature OnCameraZoom;
	UPROPERTY(BlueprintAssignable)
	FOnPhotoTakenSignature OnPhotoTaken;
	UPROPERTY(BlueprintAssignable)
	FOnCatPhotoTakenSignature OnCatPhotoTakenEvent;
	
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
	void FocusCamera(EZoomLevel NewZoomLevel);
	void FocusCamera(int value);
	
	UFUNCTION(BlueprintCallable)
	bool IsActorWithinFocusRegion(AActor* Actor) const;
	
	float GetNormalisedFOVScale();
	
private:
	void AdjustCameraZoom(EZoomLevel NewZoomLevel);
	float GetFOVLevel(EZoomLevel NewZoomLevel);
};
