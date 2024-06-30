// Fill out your copyright notice in the Description page of Project Settings.


#include "Cat.h"

#include "MainGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACat::ACat()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider->SetupAttachment(RootComponent);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Mesh->SetupAttachment(BoxCollider);

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Controller"));

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIController = Cast<AAIController>(GetController());
}

// Called when the game starts or when spawned
void ACat::BeginPlay()
{
	Super::BeginPlay();

	Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->RegisterCat(this);

	// Set Model based on Cat Type
	Mesh->SetSkeletalMesh(GetMesh());
	
	StartingPosition = GetActorLocation();
	RandomRoam();
}

// Called every frame
void ACat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsRoaming)
	{
		FVector movementDirection = GetActorForwardVector();
		FVector newPosition = GetActorLocation() + (movementDirection * PawnMovement->MaxSpeed * DeltaTime);
		SetActorLocation(newPosition);
		
		float distance = FVector::Distance(GetActorLocation(), RoamPosition);
		//GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, "Distance to Position: " + FString::SanitizeFloat(distance));
		if (distance <= RoamReachDistance)
		{
			IsRoaming = false;
			OnMovementComplete();
		}
	}
}

void ACat::RandomRoam()
{
	if (!CanRoam)
		return;

	if (RoamRegion == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, "ERROR: Cat is trying to Roam with no Roam Region Set");
		return;
	}
		
	FVector origin;
	FVector boxExtent;
	RoamRegion->GetActorBounds(false, origin, boxExtent);
	
	FVector randomPosition = UKismetMathLibrary::RandomPointInBoundingBox(origin, boxExtent);
	FVector roamPosition = FVector(randomPosition.X, randomPosition.Y, StartingPosition.Z);
	//DrawDebugLine(GetWorld(), GetActorLocation(), roamPosition, FColor::Red, false, 2.0f, 0.0f, 4.0f);

	RoamPosition = roamPosition;
	OnNewRoamPosition(roamPosition);
	IsRoaming = true;
}

void ACat::OnMovementComplete()
{
	float waitDuration = UKismetMathLibrary::RandomFloatInRange(MinRoamWaitDuration, MaxRoamWaitDuration);
	RoamTimer = waitDuration;
	GetWorld()->GetTimerManager().SetTimer(RoamTimerHandle, [this]()
	{
		RoamTimer -= UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		if (RoamTimer <= 0.0f)
		{
			RandomRoam();
			GetWorld()->GetTimerManager().ClearTimer(RoamTimerHandle);
		}
		
	}, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), true);
}

USkeletalMesh* ACat::GetMesh()
{
	for (FCatType CatMesh : CatMeshes)
	{
		if (CatMesh.Type == CatType)
		{
			return CatMesh.Mesh;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, "ERROR: Can't find Mesh for Cat");
	return nullptr;
}

