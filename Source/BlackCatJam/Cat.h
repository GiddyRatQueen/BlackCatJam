// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Cat.generated.h"

UENUM(BlueprintType)
enum class ECatType : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	FatCat UMETA(DisplayName = "Fat Cat"),
	WavingCat UMETA(DisplayName = "Waving Cat"),
	SunBathingCat UMETA(DisplayName = "Sun Bathing Cat"),
	TrashCat UMETA(DisplayName = "Trash Cat"),
	BreadCat UMETA(DisplayName = "Bread Cat"),
	HatCat UMETA(DisplayName = "Hat Cat"),
	FishingCat UMETA(DisplayName = "Fishing Cat"),
	BlackCat UMETA(DisplayName = "Black Cat")
};

UCLASS()
class BLACKCATJAM_API ACat : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
    class USkeletalMeshComponent* Mesh;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECatType CatType;
	
public:
	// Sets default values for this character's properties
	ACat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
