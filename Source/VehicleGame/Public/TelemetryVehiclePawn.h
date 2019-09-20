#pragma once

#include "CoreMinimal.h"
#include "Pawns/BuggyPawn.h"

#include "TelemetryVehiclePawn.generated.h"

UCLASS()
class VEHICLEGAME_API ATelemetryVehiclePawn : public ABuggyPawn
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly)
	class UVehicleSimData* VehicleSimData;
	UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly)
	class UTelemetryComponent* TelemetryComponent;

	ATelemetryVehiclePawn(const FObjectInitializer& ObjectInitializer);

	/*Implementando questo metodo si garantisce una callback quando si attiva lo showdebug*/
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
};