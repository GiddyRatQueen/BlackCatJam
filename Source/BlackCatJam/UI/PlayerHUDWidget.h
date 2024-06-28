#pragma once

#include "CoreMinimal.h"
#include "BlackCatJam/Player/SnapCamera.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "PlayerHUDWidget.generated.h"


UCLASS()
class BLACKCATJAM_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool IsCameraFocusing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UCanvasPanel* CameraCanvas;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FVector2D GetCameraViewport();
	
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCameraZoom(EZoomLevel ZoomLevel);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPhotoTaken();

	UFUNCTION(BlueprintImplementableEvent)
	void SetCameraViewport();
	
private:
	UFUNCTION()
	void CameraZoomCallback(EZoomLevel ZoomLevel);
	
};
