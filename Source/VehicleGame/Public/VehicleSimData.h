#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleSimData.generated.h"

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