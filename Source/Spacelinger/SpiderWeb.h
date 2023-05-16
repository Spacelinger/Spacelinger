// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "SpiderWeb.generated.h"


UCLASS(config = Game)
class SPACELINGER_API ASpiderWeb : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	ASpiderWeb();

	UPROPERTY(VisibleAnywhere)
		UCableComponent* CableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		USphereComponent* StartLocationCable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		UPhysicsConstraintComponent* ConstraintComp;

	UMaterial* StoredMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
