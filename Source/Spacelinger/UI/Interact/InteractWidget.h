// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

class ASLSoldier;

UCLASS()
class SPACELINGER_API UInteractWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	FLinearColor WidgetBaseColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	FLinearColor WidgetFocusedColor = FLinearColor(1.0f, .0f, .0f, 1.0f);

	UFUNCTION(BlueprintCallable)
	virtual const FLinearColor GetWidgetColor();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	AActor* OwningActor = nullptr;
};
