#pragma once

#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoTakenSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraFocusSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraUnFocusSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrackStartSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrackEndSignature);

class APlayerCharacter;
class USnapCamera;

UCLASS()
class BLACKCATJAM_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPhotoTakenSignature OnPhotoTakenEvent;

	UPROPERTY(BlueprintAssignable)
	FOnCameraFocusSignature OnCameraFocusEvent;
	
	UPROPERTY(BlueprintAssignable)
	FOnCameraUnFocusSignature OnCameraUnFocusEvent;

	UPROPERTY(BlueprintAssignable)
	FOnTrackStartSignature OnTrackStart;

	UPROPERTY(BlueprintAssignable)
	FOnTrackEndSignature OnTrackEnd;

private:
	APlayerCharacter* PlayerCharacter;
	USnapCamera* PlayerSnapCamera;

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
	
protected:
	virtual void BeginPlay() override;
	
	void OnPhotoTaken() const;
	UFUNCTION()
	void OnCameraFocus();
	UFUNCTION()
	void OnCameraUnFocus();

	UFUNCTION()
	void OnPlayerReachedEndOfTrack();
};