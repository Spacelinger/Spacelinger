// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LifeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageReceived, int, Damage, AActor*, DamageDealer);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealReceived, int, Heal, AActor*, Healer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDie, AActor*, Killer);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACELINGER_API ULifeComponent : public UActorComponent
{

    GENERATED_BODY()

public:
    ULifeComponent();

    virtual void InitializeComponent() override;

    //virtual void InitializeComponent() override;

    UFUNCTION(BlueprintCallable)
        void ReceiveDamage(int Damage, AActor* DamageDelaer);

    //void ReceiveHeal(int Heal, AActor* Healer); // Heal

    UPROPERTY(BlueprintAssignable)
        FOnDamageReceived OnDamageReceivedDelegate;

    // UPROPERTY(BlueprintAssignable)
    //    FOnHealReceived OnHealReceived;

    UPROPERTY(BlueprintAssignable)
        FOnDie OnDieDelegate;

    int GetMaxLife() const { return MaxLife; }
    int GetCurrentLife() const { return CurrentLife; }


protected:

    void OnDie(AActor* Killer);

    void OnDestroyTimer();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        int CurrentLife = 110;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        int MaxLife = 110;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        int TimeToDestroyAfterDeath = 3.0f;

    FTimerHandle DestroyTimerHandle;

};