#pragma once

#include "Cat.h"
#include "GameFramework/GameModeBase.h"
#include "Player/SnapCamera.h"
#include "MainGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrackStartSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrackEndSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCatDetectedSignature);

class APlayerCharacter;
class USnapCamera;

UCLASS()
class BLACKCATJAM_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCameraZoomSignature OnCameraZoomEvent;
	
	UPROPERTY(BlueprintAssignable)
	FOnPhotoTakenSignature OnPhotoTakenEvent;

	UPROPERTY(BlueprintAssignable)
	FOnTrackStartSignature OnTrackStart;

	UPROPERTY(BlueprintAssignable)
	FOnTrackEndSignature OnTrackEnd;

private:
	APlayerCharacter* PlayerCharacter;
	USnapCamera* PlayerSnapCamera;

	TArray<ACat*> Cats;
	TArray<ECatType> PhotographedCats;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track")
	bool TrackStarted;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track")
	bool TrackEnded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track")
	bool IsMovingAlongTrack;
	
public:
	AMainGameMode();

	UFUNCTION(BlueprintCallable)
	void StartTrack();

	UFUNCTION(BlueprintCallable)
	void EndTrack();

	void RegisterCat(ACat* cat);
	TArray<ACat*> GetListOfCats();
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnCatDetected(ACat* Cat);

	UFUNCTION()
	void OnPlayerReachedEndOfTrack();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCatPhotograph(ECatType CatType);

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewCatPhotograph(ECatType CatType);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMissionComplete();

private:
	UFUNCTION()
	void OnCameraZoom(EZoomLevel ZoomLevel);

	UFUNCTION()
	void OnPhotoTaken();

	bool AreAllCatsPhotographed() const;
};