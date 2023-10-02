// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "UI/Soldier/SLDetectionWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Soldier/SoldierAIController.h"
#include "Components/CanvasPanel.h"
#include "Spider/Slime_A.h"
#include "GameFramework/SpringArmComponent.h"

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
	
	// Check if the biggest element from the SoldierAwarenessMap is bigger than .01f
	if (it->second > .01f)
	{
		// And make sure that only the wanted actor will trigger sounds
		if (Actor == OwningActor && !Actor->IsDead())
		{
			AudioManager = Actor->GetAudioManager();
			// Check if the awareness of the actor is going up
			if (!CurrentBarFillingSound)
			{
				// Set the volume multiplier to increase or decrease with the awareness
				CurrentBarFillingSound = UGameplayStatics::SpawnSound2D(this, BarFillingSound, 0.45f, 3*it->second);
			} else
			{
				CurrentBarFillingSound->SetVolumeMultiplier(it->second);
			}
			if (it->second >= 1.f)
			{
				if (!CurrentDetectionSound)
				{
					ActorRecentlyAware = true;
					AudioManager->PlayChaseMusic();
					CurrentDetectionSound = UGameplayStatics::SpawnSound2D(this, DetectionSound, 0.25f);
					AudioManager->Soldier_VoiceCue(Actor->GetActorLocation(), Actor->GetActorRotation());
				}
				if (CurrentBarFillingSound)
				{
					CurrentBarFillingSound->FadeOut(1.5f, 0.f);
				}
			}
		}
	}
	else
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
		if (ActorRecentlyAware)
		{
			AudioManager = Actor->GetAudioManager();
			if (Actor->IsDead())
			{
				AudioManager->SoldierDeathAudioReaction();
			} else
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
	

ASoldierAIController* USLDetectionWidget::GetAIController(AActor *Actor) const {
	APawn* AsPawn = Cast<APawn>(Actor);
	if (AsPawn) {
		return Cast<ASoldierAIController>(AsPawn->GetController());
	}
	return nullptr;
}
