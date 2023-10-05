// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "SLSpiderWebBall.generated.h"

UCLASS()
class SPACELINGER_API ASLSpiderWebBall : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASLSpiderWebBall();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Add your own functions and variables here
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
    UStaticMeshComponent* SphereMesh;

    UFUNCTION()
    void ResetTimeLine();

private:
    FTimeline MyTimeline;
    UPROPERTY(EditAnywhere)
    UCurveFloat* MyCurve;

    UFUNCTION()
    void HandleProgress(float Value);
};

