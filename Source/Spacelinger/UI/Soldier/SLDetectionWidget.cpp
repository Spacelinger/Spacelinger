// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "UI/Soldier/SLDetectionWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Soldier/SLSoldier.h"
#include "Soldier/SoldierAIController.h"
#include "Components/CanvasPanel.h"
#include "Spider/Slime_A.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

float USLDetectionWidget::GetBarPercent() const {
	if (ASLSoldier *SoldierActor = Cast<ASLSoldier>(OwningActor)) {
		// If stunned awareness is max, so we show how long until it gets unstunned
		// otherwise we just show it's current awareness
		if (SoldierActor->IsStunned()) {
			return SoldierActor->GetRemainingTimeToUnstunAsPercentage();
		}

		if (ASoldierAIController *AIController = GetAIController(OwningActor)) {
			return AIController->CurrentAwareness;
		}
	}
	
	return .0f;
}

ESlateVisibility USLDetectionWidget::GetBarVisibility() {
	PlaySounds();
	return IsActorAware(OwningActor) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden;
}

ESlateVisibility USLDetectionWidget::GetBarVisibilityOffscreen() {
	PlaySounds();
	if (!OwningActor || !IsActorAware(OwningActor)) 
		return ESlateVisibility::Hidden;

	// Check if it's offscren
	UWorld* World = GetWorld();
	if (!World) return ESlateVisibility::Hidden;
	APlayerController *PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return ESlateVisibility::Hidden;
	ASlime_A *PlayerCharacter = Cast<ASlime_A>(PlayerController->GetPawn());
	if (!PlayerCharacter) return ESlateVisibility::Hidden;

	ESlateVisibility Result = ESlateVisibility::Hidden;
	FVector2D OutScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(PlayerController, OwningActor->GetActorLocation(), OutScreenPosition)) {
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
		bool IsOnScreen = (OutScreenPosition.X > .0f && OutScreenPosition.X <= ViewportSize.X &&
					       OutScreenPosition.Y > .0f && OutScreenPosition.Y <= ViewportSize.Y);
		if (!IsOnScreen) {
			Result = ESlateVisibility::HitTestInvisible;
		}
	}

	// Set rotation to point to actor if we are going to show the widget
	if (Result == ESlateVisibility::HitTestInvisible) {
		FRotator LookAtRotator = FRotationMatrix::MakeFromX(OwningActor->GetActorLocation() - PlayerCharacter->GetActorLocation()).Rotator();
		FRotator CameraRotation = PlayerCharacter->GetCameraBoom()->GetTargetRotation();
		FRotator Delta = LookAtRotator - CameraRotation;
		Delta.Normalize();
		
		RotationPanel->SetRenderTransformAngle(Delta.Yaw);
	}

	return Result;
}

FLinearColor USLDetectionWidget::GetBarColor() const {
	if (ASoldierAIController *AIController = GetAIController(OwningActor)) {
		if (AIController -> IsStunned())
		{
			return StunnedColor;
		}
		return AIController->bIsAlerted ? AlertedColor : DetectedColor;
	}
	return DetectedColor;
}

// NOTE(Sergi): Not used yet since we can't bind this function to the background color
// I'll figure out something eventually
FLinearColor USLDetectionWidget::GetBarBackgroundColor() const {
	if (ASoldierAIController *AIController = GetAIController(OwningActor)) {
		if (AIController -> IsStunned()) {
			return AlertedColor;
		}
	}
	return DefaultBackgroundColor;
}

bool USLDetectionWidget::IsActorAware(AActor *Actor) {
	if (ASoldierAIController *AIController = GetAIController(Actor)) {
		SoldierAwarenessMap[Actor] = AIController->CurrentAwareness;
		return AIController->CurrentAwareness > .01f;
	}
	return false;
}

void USLDetectionWidget::PlaySounds()
{
	// Get the pair of elements with the biggest value in the map
	auto it = std::max_element(SoldierAwarenessMap.begin(), SoldierAwarenessMap.end(),
		[](const std::pair<AActor*, float>& p1, const std::pair<AActor*, float>& p2) {
			return p1.second < p2.second; });
	
	// Check if the biggest element from the SoldierAwarenessMap is bigger than .01f
	if (it->second > .01f)
	{
		// Get the actor associated to the biggest value in the map
		AActor* Actor = it->first;

		// Check if the awareness of the actor is going up
		if (!CurrentBarFillingSound)
		{
			// Set the volume multiplier to increase or decrease with the awareness
			CurrentBarFillingSound = UGameplayStatics::SpawnSound2D(this, BarFillingSound, 0.25f, 3*it->second);
		} else
		{
			CurrentBarFillingSound->SetVolumeMultiplier(it->second);
		}
		if (it->second >= 1.f)
		{
			if (!CurrentDetectionSound)
			{
				CurrentChaseMusic = UGameplayStatics::SpawnSound2D(this, ChaseMusic, 0.05f);
				CurrentDetectionSound = UGameplayStatics::SpawnSound2D(this, DetectionSound, 0.25f);
			}
			if (CurrentBarFillingSound)
			{
				CurrentBarFillingSound->FadeOut(1.5f, 0.f);
			}
		}
	} else
	{
		if (CurrentBarFillingSound)
		{
			CurrentBarFillingSound->FadeOut(1.0f, 0.f);
            CurrentBarFillingSound = nullptr;
		}
		if (CurrentDetectionSound)
		{
			CurrentDetectionSound = nullptr;
		}
		if (CurrentChaseMusic)
        {
            CurrentChaseMusic->FadeOut(2.0f, 0.f);
            if (!CurrentChaseMusic->bIsFadingOut)
            {
                CurrentChaseMusic = nullptr;
            }
        }
	}
}

ASoldierAIController* USLDetectionWidget::GetAIController(AActor *Actor) const {
	APawn* AsPawn = Cast<APawn>(Actor);
	if (AsPawn) {
		return Cast<ASoldierAIController>(AsPawn->GetController());
	}
	return nullptr;
}
