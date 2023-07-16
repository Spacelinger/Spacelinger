// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "UI/Soldier/SLDetectionWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Soldier/SoldierAIController.h"
#include "Components/CanvasPanel.h"
#include "Slime_A.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

float USLDetectionWidget::GetBarPercent() const {
	if (ASoldierAIController *AIController = GetAIController(OwningActor)) {
		return AIController->CurrentAwareness;
	}
	return .0f;
}

ESlateVisibility USLDetectionWidget::GetBarVisibility() const {
	return IsActorAware(OwningActor) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden;
}

ESlateVisibility USLDetectionWidget::GetBarVisibilityOffscreen() const {
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

bool USLDetectionWidget::IsActorAware(AActor *Actor) const {
	if (ASoldierAIController *AIController = GetAIController(Actor)) {
		return AIController->CurrentAwareness > .01f;
	}
	return false;
}

ASoldierAIController* USLDetectionWidget::GetAIController(AActor *Actor) const {
	APawn* AsPawn = Cast<APawn>(Actor);
	if (AsPawn) {
		return Cast<ASoldierAIController>(AsPawn->GetController());
	}
	return nullptr;
}
