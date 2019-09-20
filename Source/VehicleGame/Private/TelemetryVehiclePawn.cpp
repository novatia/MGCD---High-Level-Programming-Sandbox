#include "VehicleGame.h"

#include "TelemetryVehiclePawn.h"
#include "TelemetryComponent.h"

#include "VehicleSimData.h"
//CONSOLE: showdebug
ATelemetryVehiclePawn::ATelemetryVehiclePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	VehicleSimData = CreateDefaultSubobject<UVehicleSimData>(TEXT("VehicleSimData"));
	TelemetryComponent = CreateDefaultSubobject<UTelemetryComponent>(TEXT("TelemetryComp"));
}

//la struttura viene passata e si può leggere il toggle che abbiamo definito.
void ATelemetryVehiclePawn::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	static FName NAME_Telemetry = FName(TEXT("Telemetry"));

	if (DebugDisplay.IsDisplayOn(NAME_Telemetry))
	{
		TelemetryComponent->DrawTelemetry(Canvas, YL, YPos);
	}
}