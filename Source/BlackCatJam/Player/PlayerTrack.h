// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SplineMeshActor.h"
#include "PlayerTrack.generated.h"

/**
 * 
 */
UCLASS()
class BLACKCATJAM_API APlayerTrack : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Track", meta=(AllowPrivateAccess = "true"))
	class USplineComponent* SplineTrack;

public:
	APlayerTrack();

	UFUNCTION(BlueprintCallable)
	FVector GetPositionOnTrack(float distance);

	bool HasReachedEndOfTrack(float distance);
};
