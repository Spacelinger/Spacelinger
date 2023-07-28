#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "SpiderProjectile.generated.h"

UCLASS()
class SPACELINGER_API ASpiderProjectile : public AActor
{
    GENERATED_BODY()

public:
    ASpiderProjectile();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Projectile")
        float InitialSpeed = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Components")
        USphereComponent* SphereCollider;
    UPROPERTY(VisibleAnywhere, Category = "Components")
        UProjectileMovementComponent* ProjectileMovementComponent;

private:
    

    UPROPERTY(VisibleAnywhere, Category = "Components")
        UStaticMeshComponent* SphereMesh;

    // Function to handle the sphere collider's collision with other actors
    UFUNCTION()
        void OnSphereColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
