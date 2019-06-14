#include "VehicleGame.h"
#include "VehicleSimData.h"

#include "WheeledVehicle.h"
#include "WheeledVehicleMovementComponent.h"

#include "PhysXPublic.h"
#include "PhysXVehicleManager.h"

UVehicleSimData::UVehicleSimData()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleSimData::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AWheeledVehicle* Vehicle = Cast<AWheeledVehicle>(GetOwner());

	if (Vehicle != nullptr)
	{
		UWheeledVehicleMovementComponent* VehicleMovement = Vehicle->GetVehicleMovementComponent();

		if (VehicleMovement != nullptr)
		{
			physx::PxVehicleDrive* PVehicleDrive = VehicleMovement->PVehicleDrive;

			if (PVehicleDrive != nullptr)
			{
				const PxVec3 PLinearVelocity = PVehicleDrive->getRigidDynamicActor()->getLinearVelocity();
				const PxVec3 PAngularVelocity = PVehicleDrive->getRigidDynamicActor()->getAngularVelocity();

				ChassisSimData.LinearVelocity = P2UVector(PLinearVelocity);
				ChassisSimData.AngularVelocity = P2UVector(PAngularVelocity);

				ChassisSimData.Speed = ChassisSimData.LinearVelocity.Size();
			}

			for (int32 WheelIndex = 0; WheelIndex < 4; ++WheelIndex)
			{
				FWheelSimData& WheelData = WheelSimData[WheelIndex];

				WheelData.RotationAngle = PVehicleDrive->mWheelsDynData.getWheelRotationAngle(WheelIndex);
				WheelData.RotationSpeed = PVehicleDrive->mWheelsDynData.getWheelRotationSpeed(WheelIndex);
			}
		}
	}
}