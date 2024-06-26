#include "BlackCatJam/Player/PlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SnapCamera.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	Sensitivity = 1.0f;
	
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

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D lookAxis = Value.Get<FVector2D>();
	lookAxis.Normalize();

	float interpolatedSensitivity = FMath::Lerp(Sensitivity, FocusSensitivity, PlayerCamera->GetNormalisedFOVScale());
	lookAxis *= interpolatedSensitivity * UGameplayStatics::GetWorldDeltaSeconds(this);
	
	if (Controller != nullptr)
	{
		AddControllerYawInput(lookAxis.X);
		AddControllerPitchInput(lookAxis.Y);
	}
}

void APlayerCharacter::TakePhoto()
{
	PlayerCamera->TakePhoto();
}

void APlayerCharacter::FocusCamera()
{
	PlayerCamera->FocusCamera();
}
