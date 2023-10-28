// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/TogglableVisualElement.h"
#include "Audio/SpacelingerAudioComponent.h"
#include "SLLaserPuzzle.generated.h"

class UStaticMeshComponent;
class UParticleSystemComponent;
class ASpiderWeb;
class ASLLockedDoor;
class UBoxComponent;

UENUM(BlueprintType)
enum class SLParticleParameterType : uint8 {
	Bool = 0,
	Float,
	Vector,
};

USTRUCT()
struct FSLParticleParameter {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName Key;
	UPROPERTY(EditAnywhere)
	SLParticleParameterType Type = SLParticleParameterType::Bool;
	UPROPERTY(EditAnywhere, meta=(EditCondition="Type == SLParticleParameterType::Bool", EditConditionHides))
	bool bEnabled = false;
	UPROPERTY(EditAnywhere, meta=(EditCondition="Type == SLParticleParameterType::Float", EditConditionHides))
	float FloatValue = 0.0f;
	UPROPERTY(EditAnywhere, meta=(EditCondition="Type == SLParticleParameterType::Vector", EditConditionHides))
	FVector VectorValue = FVector::ZeroVector;
};


UCLASS()
class SPACELINGER_API ASLLaserPuzzle: public AActor
{
	GENERATED_BODY()
	
public:
	ASLLaserPuzzle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *MeshLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *MeshRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent *BeamTop;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent *BeamBot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent *TriggerLeftTop;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent *TriggerRightTop;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent *TriggerLeftBot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent *TriggerRightBot;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	bool bActiveBeamTop = false;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	bool bActiveBeamBot = false;

	// Parameters when we put the right mesh closer or further away. Values are going to be adjusted for the actual distance between meshes
	UPROPERTY(EditAnywhere, Category = "Spacelinger")
	TArray<FSLParticleParameter> LengthParticleParameters;
	// Parameters when the puzzle is solved
	UPROPERTY(EditAnywhere, Category = "Spacelinger")
	TArray<FSLParticleParameter> SolvedParticleParameters;
	// Parameters when the puzzle isn't solved
	UPROPERTY(EditAnywhere, Category = "Spacelinger")
	TArray<FSLParticleParameter> UnsolvedParticleParameters;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger", Meta = (MakeEditWidget = true))
	FVector MeshRightLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	float OffsetMeshes = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	float ConnectedRadius = 75.0f;

	// Elements to trigger once the top puzzle is solved
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	TArray<ATogglableVisualElement*> TopVisualElements;
	// Elements to trigger once the bottom puzzle is solved
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	TArray<ATogglableVisualElement*> BotVisualElements;
	// Doors to unlock once the full puzzle is solved
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	TArray<ASLLockedDoor*> AssociatedDoors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	UGameInstance *GameInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|Audio", meta = (AllowPrivateAccess = "true"))
	USpacelingerAudioComponent* AudioManager = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ASpiderWeb *LastAttachedWeb;
	UFUNCTION()
	void WebEndConnection(ASpiderWeb *Web);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "QuestLog")
		void TopBeamActivated();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "QuestLog")
		void BotBeamActivated();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "QuestLog")
		void PuzzleSolved();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	float BeamLength = 0.0f;

	void SetBeamVisuals(UParticleSystemComponent *Beam, TArray<FSLParticleParameter> Parameters, float Magnitude = 1.0f, bool RefreshParticleSytem = true);
	void UpdateAssociatedVisualElements();
	void PlayFinalAudio();
	void PlayElectricitySFX(FVector Location, FRotator Rotation);

};
