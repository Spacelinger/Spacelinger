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

class ASlime_A;

UCLASS(config = Game)
class SPACELINGER_API ASpiderWeb : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	ASpiderWeb();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		UCableComponent* CableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		USphereComponent* StartLocationCable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		UPhysicsConstraintComponent* ConstraintComp;


	void setFuturePosition(FVector futurePosition, ASlime_A* spider);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		FVector initialPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bSetPosition = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		ASlime_A* spider;

	UMaterial* StoredMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
