// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

// SpiderWebBall.cpp

#include "SLSpiderWebBall.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"

// Sets default values
ASLSpiderWebBall::ASLSpiderWebBall()
{

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Initialize your own components and variables here
    SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
    SphereMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASLSpiderWebBall::BeginPlay()
{
    Super::BeginPlay();

    if (MyCurve)
    {
        FOnTimelineFloat ProgressFunction;

        ProgressFunction.BindUFunction(this, FName("HandleProgress"));

        MyTimeline.AddInterpFloat(MyCurve, ProgressFunction);
        MyTimeline.SetLooping(false);
        MyTimeline.PlayFromStart();
    }
}

// Called every frame
void ASLSpiderWebBall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MyTimeline.TickTimeline(DeltaTime);

    // Genera una rotación aleatoria
    FRotator RandomRotation = FRotator(FMath::RandRange(-180, 180), FMath::RandRange(-180, 180), FMath::RandRange(-180, 180));

    // Aplica la rotación aleatoria a SphereMesh
    SphereMesh->AddRelativeRotation(RandomRotation * DeltaTime);
}

void ASLSpiderWebBall::HandleProgress(float Value)
{
    SphereMesh->SetWorldScale3D(FVector(.1f + Value));
}

void ASLSpiderWebBall::ResetTimeLine() {

    MyTimeline.PlayFromStart();
}
