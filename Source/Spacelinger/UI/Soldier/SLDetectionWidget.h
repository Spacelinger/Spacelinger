// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <map>
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Soldier/SLSoldier.h"
#include "SLDetectionWidget.generated.h"

class ASoldierAIController;
class UCanvasPanel;

UCLASS()
class SPACELINGER_API USLDetectionWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	FLinearColor DetectedColor = FLinearColor(1.0f, 0.86f, 0.0f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	FLinearColor AlertedColor = FLinearColor(0.9f, 0.04f, 0.04f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	FLinearColor StunnedColor = FLinearColor(0, 235, 250, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	FLinearColor DefaultBackgroundColor = FLinearColor(127, 127, 127, 1);

protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	UCanvasPanel *RotationPanel;
	std::map<ASLSoldier*, float> SoldierAwarenessMap; // [!!!!!] This should be a TEMPORARY solution since we'll need some sort of hive mind for the AI, not every widget should know about every soldier
	std::map<ASLSoldier*, float> MostAwareSoldier;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	ASLSoldier *OwningActor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * BarFillingSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * DetectionSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * ChaseMusic; // This 100% should NOT be here
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierVoiceSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentBarFillingSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentDetectionSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentChaseMusic;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentSoldierVoiceSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|Audio", meta = (AllowPrivateAccess = "true"))
	USpacelingerAudioComponent* AudioManager = nullptr;

	// Bound functions
	UFUNCTION(BlueprintCallable)
	float GetBarPercent() const;
	UFUNCTION(BlueprintCallable)
	FLinearColor GetBarColor() const;
	UFUNCTION(BlueprintCallable)
	FLinearColor GetBarBackgroundColor() const;
	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetBarVisibility();
	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetBarVisibilityOffscreen();

	// Helper functions
	UFUNCTION(BlueprintCallable)
	bool IsActorAware(ASLSoldier *Actor);
	UFUNCTION(BlueprintCallable)
	void PlaySounds();

private:
	ASoldierAIController* GetAIController(AActor *Actor) const;

protected:
	//void NativeOnInitialized() override;
};
