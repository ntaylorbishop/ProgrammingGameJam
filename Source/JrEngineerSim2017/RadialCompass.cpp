// Fill out your copyright notice in the Description page of Project Settings.

#include "JrEngineerSim2017.h"
#include "RadialCompass.h"





void UHUDBlueprintLibrary::FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const float EdgePercent, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool &bIsOnScreen)
{
	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;
	FVector2D *ScreenPosition = new FVector2D();

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);

	if (!World) return;

	APlayerController* PlayerController = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);
	ACharacter *PlayerCharacter = static_cast<ACharacter *> (PlayerController->GetPawn());

	if (!PlayerCharacter) return;


	FVector Forward = PlayerCharacter->GetActorForwardVector();
	FVector Offset = (InLocation - PlayerCharacter->GetActorLocation()).SafeNormal();

	float DotProduct = FVector::DotProduct(Forward, Offset);
	bool bLocationIsBehindCamera = (DotProduct < 0);



	PlayerController->ProjectWorldLocationToScreen(InLocation, *ScreenPosition);

	// Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.
	if (ScreenPosition->X >= 0.f && ScreenPosition->X <= ViewportSize.X
		&& ScreenPosition->Y >= 0.f && ScreenPosition->Y <= ViewportSize.Y)
	{
		OutScreenPosition = *ScreenPosition;
		bIsOnScreen = true;
		return;
	}

	*ScreenPosition -= ViewportCenter;

	float AngleRadians = FMath::Atan2(ScreenPosition->Y, ScreenPosition->X);
	AngleRadians -= FMath::DegreesToRadians(90.f);

	OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;

	float Cos = cosf(AngleRadians);
	float Sin = -sinf(AngleRadians);

	ScreenPosition = new FVector2D(ViewportCenter.X + (Sin * 150.f), ViewportCenter.Y + Cos * 150.f);

	float m = Cos / Sin;

	FVector2D ScreenBounds = ViewportCenter * EdgePercent;

	if (Cos > 0)
	{
		ScreenPosition = new FVector2D(ScreenBounds.Y / m, ScreenBounds.Y);
	}
	else
	{
		ScreenPosition = new FVector2D(-ScreenBounds.Y / m, -ScreenBounds.Y);
	}

	if (ScreenPosition->X > ScreenBounds.X)
	{
		ScreenPosition = new FVector2D(ScreenBounds.X, ScreenBounds.X*m);
	}
	else if (ScreenPosition->X < -ScreenBounds.X)
	{
		ScreenPosition = new FVector2D(-ScreenBounds.X, -ScreenBounds.X*m);
	}

	*ScreenPosition += ViewportCenter;

	OutScreenPosition = *ScreenPosition;

}
