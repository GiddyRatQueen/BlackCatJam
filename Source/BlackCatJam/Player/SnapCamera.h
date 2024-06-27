#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "SnapCamera.generated.h"

DECLARE_DELEGATE(FOnPhotoSnapSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCatPhotoTakenSignature, class ACat*, Cat);

UENUM(BlueprintType)
enum class EZoomLevel : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Far UMETA(DisplayName = "Far"),
	VeryFar UMETA(DisplayName = "Very Far"),
};

class USceneCaptureComponent2D;
class UCurveFloat;

UCLASS(ClassGroup=Camera, meta=(BlueprintSpawnableComponent))
class BLACKCATJAM_API USnapCamera : public UCameraComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
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
	
	FOnPhotoSnapSignature OnPhotoTaken;
	FOnCatPhotoTakenSignature OnCatPhotoTaken;
	
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

	UFUNCTION(BlueprintImplementableEvent)
	void OnCameraZoomIn();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCameraZoomOut();
	
	UFUNCTION(BlueprintCallable)
	bool IsActorWithinFocusRegion(AActor* Actor) const;
	
	float GetNormalisedFOVScale();
	
private:
	void AdjustCameraZoom(EZoomLevel NewZoomLevel);
	float GetFOVLevel(EZoomLevel NewZoomLevel);
};
