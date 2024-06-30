// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Cat.generated.h"

UENUM(BlueprintType)
enum class ECatType : uint8
{
	Black UMETA(DisplayName = "Black"),
	Black2 UMETA(DisplayName = "Black 2"),
	Brown UMETA(DisplayName = "Brown"),
	Ginger UMETA(DisplayName = "Ginger"),
	Gold UMETA(DisplayName = "Gold"),
	Grey UMETA(DisplayName = "Grey"),
	Mono UMETA(DisplayName = "Mono"),
	White UMETA(DisplayName = "White"),
	Winter UMETA(DisplayName = "Winter"),
};

USTRUCT(BlueprintType)
struct FCatType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECatType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* Mesh;
};

UCLASS()
class BLACKCATJAM_API ACat : public APawn
{
	GENERATED_BODY()

	// -- Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
    class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	class UFloatingPawnMovement* PawnMovement;
	
	// -- Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roaming", meta=(AllowPrivateAccess = "true"))
	class ATriggerBox* RoamRegion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roaming", meta=(AllowPrivateAccess = "true"))
	bool CanRoam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roaming", meta=(AllowPrivateAccess = "true"))
	FVector RoamPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roaming", meta=(AllowPrivateAccess = "true"))
	bool IsRoaming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roaming", meta=(AllowPrivateAccess = "true"))
	float RoamReachDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roaming", meta=(AllowPrivateAccess = "true"))
	float MinRoamWaitDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roaming", meta=(AllowPrivateAccess = "true"))
	float MaxRoamWaitDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TArray<FCatType> CatMeshes;
	
	AAIController* AIController;
	FVector StartingPosition;
	FTimerHandle RoamTimerHandle;
	float RoamTimer;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECatType CatType;
	
public:
	// Sets default values for this character's properties
	ACat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewRoamPosition(FVector Position);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void RandomRoam();
	void OnMovementComplete();

	USkeletalMesh* GetMesh();
};
