#include "VehicleGame.h"
#include "VehicleSimData.h"

#include "WheeledVehicle.h"
#include "WheeledVehicleMovementComponent.h"

#include "PhysXPublic.h"
#include "PhysXVehicleManager.h"

UVehicleSimData::UVehicleSimData()
{
	
	PrimaryComponentTick.bCanEverTick = true; //l'oggetto DEVE fare il tick, ovvero l'update
	PrimaryComponentTick.TickGroup = TG_PostPhysics;/*Qui si configura la modalità di tick del componente*/
}

/*
Questo metodo prende da un oggetto di tipo Physx driver i valori richiesti per il veicolo
Questo metodo viene fatto PER TICK ovvero per frame.

Il frame viene composto in parallelo. Anche se non sono di default multi core in unreal se non specificato.

------------TIME ----------->
| PRE PHY | DURING PHY | POST PHY | (sono 6/7)

Il tick dell'oggetto è configurabile in una qualsiasi di queste fasi.
C'è una update unica che va sempre a frame time.
Quello che si può fare è scegliere in quale delle fasi inserirci.
*/
void UVehicleSimData::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AWheeledVehicle* Vehicle = Cast<AWheeledVehicle>(GetOwner());

	if (Vehicle != nullptr)
	{

		UWheeledVehicleMovementComponent* VehicleMovement = Vehicle->GetVehicleMovementComponent();
		USceneComponent *c  = GetOwner()->GetRootComponent();
		UPrimitiveComponent *uc = Cast<UPrimitiveComponent>(c);
		FBodyInstance* BI = uc->GetBodyInstance();
		FVector velociy = BI->GetUnrealWorldVelocity_AssumesLocked();

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