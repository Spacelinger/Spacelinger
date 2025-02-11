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
class ASLSoldier;

UENUM()
enum EWebType
{
	SpiderWeb = 0	UMETA(DisplayName = "Spider Web"),
	HookWeb			UMETA(DisplayName = "Hook"),
	StunWeb			UMETA(DisplayName = "Stun"),
	TrapWeb			UMETA(DisplayName = "Trap"),
};

UCLASS(config = Game)
class SPACELINGER_API ASpiderWeb : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	ASpiderWeb();
	virtual void Tick(float DeltaTime) override;
	void ResetConstraint();
	FVector getRelativePositionPhysicsConstraint();
	FVector getVectorInConstraintCoordinates(FVector input, float Speed, float DeltaTime);


	UPROPERTY(VisibleAnywhere)
		UCableComponent* CableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		USphereComponent* StartLocationCable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		USphereComponent* EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		UPhysicsConstraintComponent* ConstraintComp;

	UFUNCTION()
		void OnEndPointCollision(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void setFuturePosition(FVector futurePosition, ASlime_A* spider, bool attached, bool bIsHook);
	void SetTrap();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		FVector initialPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bSetPosition = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bAttached = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bTrapActivated = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		FVector distanceTrap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		ASlime_A* spider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		FVector initialRelativePosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bInitialRelativePositionSet = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bTrapFinished = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bIsHook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
	TEnumAsByte<EWebType> SpiderWebType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
	ASLSoldier* Soldier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		float TimeToLive = 100.0f; // In seconds

	UMaterial* StoredMaterial;

	void SelfDestruction();	// Public because we need to be able to destroy once there are too many
	
	FTimerHandle SelfDestructTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Helper functions to get attachment locations
	// Both of these assume there is a start and end attachm locations
	FORCEINLINE FVector GetStartAttachLocation() const { return StartLocationCable->GetComponentLocation(); }
	FORCEINLINE FVector GetEndAttachLocation() const { return StartLocationCable->GetComponentLocation() + CableComponent->EndLocation; }

private:
	FName FootBoneName = "Bip001-R-Foot";
};
