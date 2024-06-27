// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackCatJam/Player/PlayerTrack.h"

#include "Components/SplineComponent.h"

APlayerTrack::APlayerTrack()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SplineTrack = CreateDefaultSubobject<USplineComponent>(TEXT("Track"));
	SplineTrack->SetupAttachment(RootComponent);
}

FVector APlayerTrack::GetPositionOnTrack(float distance)
{
	FTransform splineTransform = SplineTrack->GetTransformAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World, false);
	return splineTransform.GetLocation();
}

bool APlayerTrack::HasReachedEndOfTrack(float distance)
{
	return distance >= SplineTrack->GetSplineLength();
}
