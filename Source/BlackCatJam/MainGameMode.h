#pragma once

#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoTakenSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraFocusSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraUnFocusSignature);

class USnapCamera;

UCLASS()
class BLACKCATJAM_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnPhotoTakenSignature OnPhotoTakenEvent;

	UPROPERTY(BlueprintAssignable)
	FOnCameraFocusSignature OnCameraFocusEvent;
	
	UPROPERTY(BlueprintAssignable)
	FOnCameraUnFocusSignature OnCameraUnFocusEvent;

	USnapCamera* PlayerSnapCamera;
	
public:
	AMainGameMode();

protected:
	virtual void BeginPlay() override;
	
	void OnPhotoTaken() const;
	UFUNCTION()
	void OnCameraFocus();
	UFUNCTION()
	void OnCameraUnFocus();
};