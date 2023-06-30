// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SLGameHUD.generated.h"

/**
 * HUD used ingame
 */
UCLASS()
class SPACELINGER_API ASLGameHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|UI", meta = (AllowPrivateAccess = "true"))
	UClass *PlayerInterfaceClass;

	UPROPERTY()
	UUserWidget *PlayerInterface;

public:
	ASLGameHUD();

	UUserWidget* InstantiateWidget(UClass *InterfaceClass);

protected:
	virtual void BeginPlay() override;
};
