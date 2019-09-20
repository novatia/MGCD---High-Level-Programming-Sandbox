#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleSimData.generated.h"
/*

Classe di telemetria per classe BuggyPawn
Queste classi servono per fare una copia dei dati di telemetria che non sono uproperty. Quindi se vogliamo vederli tramite
reflection ci vuole un componente che legge i dati e li espone come uproperty.


*/

/*strutture definite per le misure del veicolo da physics*/
USTRUCT()
struct FChassisSimData
{
	GENERATED_BODY()

	UPROPERTY()
	float	Speed;
	UPROPERTY()
	FVector LinearVelocity;
	UPROPERTY()
	FVector AngularVelocity;
};


/*strutture definite per le misure delle ruote*/
USTRUCT()
struct FWheelSimData 
{
	GENERATED_BODY()

	UPROPERTY()
	float	RotationAngle;
	UPROPERTY()
	float	RotationSpeed;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VEHICLEGAME_API UVehicleSimData : public UActorComponent
{
	GENERATED_BODY()

public:	

	UVehicleSimData();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UPROPERTY()
	FChassisSimData ChassisSimData;
	UPROPERTY()
	FWheelSimData	WheelSimData[4];
};