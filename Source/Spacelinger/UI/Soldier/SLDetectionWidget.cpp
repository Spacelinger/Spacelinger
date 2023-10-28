// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "UI/Soldier/SLDetectionWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Soldier/SoldierAIController.h"
#include "Components/CanvasPanel.h"
#include "Spider/Slime_A.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"


float USLDetectionWidget::GetBarPercent() const {
		// If stunned awareness is max, so we show how long until it gets unstunned
		// otherwise we just show it's current awareness
		if (OwningActor->IsStunned()) {
			return OwningActor->GetRemainingTimeToUnstunAsPercentage();
		}

		if (ASoldierAIController *AIController = GetAIController(OwningActor)) {
			return AIController->CurrentAwareness;
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

	ASLSoldier *SoldierActor = Cast<ASLSoldier>(OwningActor);
	if (!SoldierActor) return ESlateVisibility::Hidden;

	FVector WidgetWorldPosition = SoldierActor->DetectionWidget->GetComponentLocation();
	FVector2D OutScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(PlayerController, WidgetWorldPosition, OutScreenPosition)) {
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
		bool IsOnScreen = (OutScreenPosition.X > .0f && OutScreenPosition.X <= ViewportSize.X &&
					       OutScreenPosition.Y > .0f && OutScreenPosition.Y <= ViewportSize.Y);

		if (IsOnScreen) return ESlateVisibility::Hidden;

		// Check if Soldier is behind the camera
		FVector CameraToSoldier = WidgetWorldPosition - PlayerCharacter->GetFollowCamera()->GetComponentLocation();
		if (FVector::DotProduct(PlayerCharacter->GetFollowCamera()->GetForwardVector(), CameraToSoldier) < 0)
			return ESlateVisibility::Hidden;
	}

	// Set rotation to point to actor if we are going to show the widget
	FRotator LookAtRotator = FRotationMatrix::MakeFromX(OwningActor->GetActorLocation() - PlayerCharacter->GetActorLocation()).Rotator();
	FRotator CameraRotation = PlayerCharacter->GetCameraBoom()->GetTargetRotation();
	FRotator Delta = LookAtRotator - CameraRotation;
	Delta.Normalize();
		
	RotationPanel->SetRenderTransformAngle(Delta.Yaw);

	return ESlateVisibility::HitTestInvisible;
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

bool USLDetectionWidget::IsActorAware(ASLSoldier *Actor) {
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
		[](const std::pair<ASLSoldier*, float>& p1, const std::pair<ASLSoldier*, float>& p2) {
			return p1.second < p2.second; });

	// Get the actor associated to the biggest value in the map
	ASLSoldier* Actor = it->first;
	
	// Check that any soldier is aware
	if (it != SoldierAwarenessMap.end())
	{
		// Check if the biggest element from the SoldierAwarenessMap is bigger than .01f
		if (it->second > .01f)
		{
			// And make sure that only the wanted actor will trigger sounds
			if (Actor == OwningActor && !Actor->IsDead())
			{
				AudioManager = Actor->GetAudioManager();
				ASoldierAIController* ActorController = GetAIController(Actor);
				// Check if the awareness of the actor is going up
				// and set the pitch multiplier to increase or decrease with the awareness
				// if (!ActorController -> IsAlerted())
				// {
				AudioManager -> UpdateBarFillingSound(it->second);
				// }
				
				if (ActorController -> IsStunned())
				{
					// We trigger the Voice Cue here instead of in the ASLSoldier class because we need to make
					// sure that the default flow is interrupted if the soldier is stunned
					AudioManager -> Soldier_Stunned();
				}
				else
				if (it->second >= 1.0f)
				{
					if (!ActorRecentlyAware)
					{
						ActorRecentlyAware = true;
						AudioManager->PlayChaseMusic();
						AudioManager->Soldier_VoiceCue(Actor->GetActorLocation(), Actor->GetActorRotation());
					}
					AudioManager -> StopBarFillingSound();
				}
			}
		}
		else
		{
			if (ActorRecentlyAware)
			{
				AudioManager->StopBarFillingSound();
			
				if (!Actor->IsDead())
				{
					AudioManager->Soldier_ResumePatrol();
				}
				AudioManager->StopChaseMusic();
				ActorRecentlyAware = false;
			}
		}
		if (Actor != nullptr)
		{
			if (Actor == OwningActor && Actor->IsDead())
			{
				AudioManager->StopChaseMusic();
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
