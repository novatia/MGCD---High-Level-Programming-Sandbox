#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TelemetryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VEHICLEGAME_API UTelemetryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Telemetry")
	FString		TelemetryList;
	UPROPERTY(EditDefaultsOnly, Category = "Telemetry")
	FVector2D	TelemetryRange = FVector2D(0.f, 1.f);

	// CTOR

	UTelemetryComponent();

	// UActorComponent's INTERFACE

	virtual void BeginPlay() override;

	// BUSINESS LOGIC

	void SetTelemetryQuery(const FString& InTelemetryQuery);
	void SetTelemetryGraphRange(FVector2D InRange);

	void DrawTelemetry(UCanvas* Canvas, float& YL, float& YPos);

private:
	
	void UpdateTelemetryTargets();

	// INTERNALS

	struct TelemetryTarget
	{
		UObject*	TargetObject;
		UProperty*	TargetProperty;
	};

	TArray<FString>			TelemetryArray;
	TArray<TelemetryTarget>	TelemetryTargets;
	TArray<TArray<float> >	TelemetryGraphValues;

	int32					TelemetryGraphIndex;
};
