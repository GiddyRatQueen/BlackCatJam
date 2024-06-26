#include "BlackCatJam/Player/PlayerCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerTrack.h"
#include "SnapCamera.h"
#include "BlackCatJam/MainGameMode.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	Sensitivity = 1.0f;
	TrackSpeed = 100.0f;
	MovingAlongTrack = false;
	
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerCamera = GetController()->GetPawn()->FindComponentByClass<USnapCamera>();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handles Camera Movement
	float interpolatedSensitivity = FMath::Lerp(Sensitivity, FocusSensitivity, PlayerCamera->GetNormalisedFOVScale());
	if (Controller != nullptr && LookVector != FVector2D::Zero())
	{
		FVector2D lookInput = LookVector;
		
		AddControllerYawInput(lookInput.X * interpolatedSensitivity * DeltaTime);
		AddControllerPitchInput(lookInput.Y * interpolatedSensitivity * DeltaTime);
		LookVector = FVector2D::Zero();
	}

	// Track Movement
	MoveAlongTrack(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(InputMapping, 0);
		}
	}

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		Input->BindAction(SnapPhotoAction, ETriggerEvent::Triggered, this, &APlayerCharacter::TakePhoto);
		Input->BindAction(FocusCameraAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FocusCamera);
	}
}

void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	LookVector = Value.Get<FVector2D>();
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Look Vector %s"), *LookVector.ToString()));
}

void APlayerCharacter::TakePhoto()
{
	PlayerCamera->TakePhoto();
}

void APlayerCharacter::FocusCamera()
{
	PlayerCamera->FocusCamera();
}

void APlayerCharacter::StartMovingAlongTrack()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Track Started"));
	
	DistanceAlongTrack = 0.0f;
	MovingAlongTrack = true;
}

void APlayerCharacter::MoveAlongTrack(float DeltaTime)
{
	if (Track && MovingAlongTrack)
	{
		float trackSpeed = TrackSpeed * DeltaTime;
		DistanceAlongTrack += trackSpeed;

		FVector trackPosition = Track->GetPositionOnTrack(DistanceAlongTrack);
		FVector trackOffsetPosition = FVector(trackPosition.X, trackPosition.Y, GetActorLocation().Z);
		SetActorLocation(trackOffsetPosition);

		if (Track->HasReachedEndOfTrack(DistanceAlongTrack))
		{
			MovingAlongTrack = false;
			OnReachedEndOfTrack.Broadcast();
		}
	}
}
