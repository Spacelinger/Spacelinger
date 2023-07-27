// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLLaserPuzzle.generated.h"

class UStaticMeshComponent;
class UParticleSystemComponent;
class ASpiderWeb;

UCLASS()
class SPACELINGER_API ASLLaserPuzzle: public AActor
{
	GENERATED_BODY()
	
public:
	ASLLaserPuzzle();

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	ASpiderWeb *LastAttachedWeb;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *MeshLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *MeshRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent *BeamTop;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent *BeamBot;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger", Meta = (MakeEditWidget = true))
	FVector MeshRightLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	float OffsetMeshes = 0.0f;

	UFUNCTION()
	void WebEndConnection(ASpiderWeb *Web);

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	float BeamLength = 0.0f;
};
