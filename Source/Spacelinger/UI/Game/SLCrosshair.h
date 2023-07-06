// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "SLCrosshair.generated.h"

/**
 * 
 */
UCLASS()
class SPACELINGER_API ASLCrosshair : public AHUD
{
	GENERATED_BODY()

public:
	ASLCrosshair();
	
	// Primary draw call for the HUD.
	virtual void DrawHUD() override;

protected:
	// This will be drawn at the center of the screen.
	UTexture2D* CrosshairTexture;
	
};
