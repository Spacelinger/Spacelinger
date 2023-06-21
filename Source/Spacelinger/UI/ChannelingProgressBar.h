// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChannelingProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class SPACELINGER_API UChannelingProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE class UProgressBar* GetProgressBar() { return ChannelProgressBar; }

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ChannelProgressBar;
};
