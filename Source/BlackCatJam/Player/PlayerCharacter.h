#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class USnapCamera;
class UCapsuleComponent;
class UInputMappingContext;
class UInputAction;
class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReachedEndOfTrackSignature);

UCLASS(config=Game)
class BLACKCATJAM_API APlayerCharacter : public APawn
{
	GENERATED_BODY()

	// -- Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USceneComponent* OriginPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess = "true"))
	USnapCamera* PlayerCamera;
	
	// -- Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SnapPhotoAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FocusCameraInAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FocusCameraOutAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CameraZoomAction;
	
	// -- Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess = "true"))
	float Sensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess = "true"))
	float FocusSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Track", meta=(AllowPrivateAccess = "true"))
	float TrackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Track", meta=(AllowPrivateAccess = "true"))
	bool MovingAlongTrack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Track", meta=(AllowPrivateAccess = "true"))
	bool ShouldLoopAroundTrack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track", meta=(AllowPrivateAccess = "true"))
	float DistanceAlongTrack;
	
	FVector2D LookVector;

	bool IsCameraFocusing;
	
protected:
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "Track")
	class APlayerTrack* Track;

public:
	// -- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnReachedEndOfTrackSignature OnReachedEndOfTrack;
	
public:
	// Sets default values for this pawn's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartMovingAlongTrack();

protected:
	void Look(const FInputActionValue& Value);
	
	void TakePhoto();
	void FocusCamera();
	void UnFocusCamera();
	void ZoomCamera(const FInputActionValue& Value);

private:
	void MoveAlongTrack(float DeltaTime);
};
