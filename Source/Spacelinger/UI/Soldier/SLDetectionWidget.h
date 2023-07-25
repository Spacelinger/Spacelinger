// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	AActor *OwningActor;

	// Bound functions
	UFUNCTION(BlueprintCallable)
	float GetBarPercent() const;
	UFUNCTION(BlueprintCallable)
	FLinearColor GetBarColor() const;
	UFUNCTION(BlueprintCallable)
	FLinearColor GetBarBackgroundColor() const;
	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetBarVisibility() const;
	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetBarVisibilityOffscreen() const;

	// Helper functions
	UFUNCTION(BlueprintCallable)
	bool IsActorAware(AActor *Actor) const;

private:
	ASoldierAIController* GetAIController(AActor *Actor) const;

protected:
	//void NativeOnInitialized() override;
};
