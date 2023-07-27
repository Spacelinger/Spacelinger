// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLLaserPuzzle.generated.h"

class UStaticMeshComponent;
class ASpiderWeb;

UCLASS()
class SPACELINGER_API ASLLaserPuzzle: public AActor
{
	GENERATED_BODY()
	
public:
	ASLLaserPuzzle();

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	ASpiderWeb *LastAttachedWeb;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//UStaticMeshComponent *DoorMesh;

	// This works
	//UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger", Meta = (MakeEditWidget = true))
	//FVector TestLocation = FVector::ZeroVector;

	UFUNCTION(BlueprintImplementableEvent)
	void WebEndConnection(ASpiderWeb *Web);

protected:
	virtual void BeginPlay() override;

};
