// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACELINGER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ItemNames;

	UFUNCTION(BlueprintCallable)
	void AddItem(FName NewItem);
	void FindItem(FName Item);
	void TryToRemoveItem(FName ItemToRemove);
/*
protected:
	// Called when the game starts
	virtual void BeginPlay() override;*/		//Probably not needed
};
