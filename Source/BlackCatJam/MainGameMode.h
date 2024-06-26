#pragma once

#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoTakenSignature);

class USnapCamera;

UCLASS()
class BLACKCATJAM_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnPhotoTakenSignature OnPhotoTakenEvent;

	USnapCamera* PlayerSnapCamera;
	
public:
	AMainGameMode();

protected:
	virtual void BeginPlay() override;
	
	void OnPhotoTaken() const;
};