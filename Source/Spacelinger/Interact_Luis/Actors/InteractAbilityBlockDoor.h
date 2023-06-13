// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "InteractAbilityBlockDoor.generated.h"

class UWidgetComponent;
class UBoxComponent;

UCLASS()
class SPACELINGER_API AInteractAbilityBlockDoor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	AInteractAbilityBlockDoor();

	//virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* ColliderComp = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* InteractPromptFront = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* InteractPromptBack = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int InteractPriorityValue = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BlockingTime = 5.f;

	virtual void Interact(AActor* ActorInteracting) override;
	virtual int GetInteractPriority() const { return InteractPriorityValue; }
	virtual void SetAsCandidate(bool IsCandidate) override; // TO-DO


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);
	
	void EnableBlockComponent(bool BlockState);

	void SetUIVisibility(bool IsVisible);

	void OnTimerEnded();

	//bool bIsCandidate = false;	// Probably not needed, consider removing		
	bool bIsInteracting = false;

	FTimerHandle InteractTimerHandle;

};
