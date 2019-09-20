#include "VehicleGame.h"
#include "ReflectionUtilsFunctionLibrary.h"

/**
 * @TODO
 VA MODIFICATA PER SUPPORTARE LE STRUCT E GLI ARRAY


Telemetry.Query VehicleSimData.ChassisSimData.Speed
Telemetry.Query VehicleSimData.ChassisSimData.LinearVelocity
Telemetry.Query VehicleSimData.ChassisSimData.AngularVelocity
Telemetry.Query VehicleSimData.WheelSimData[2].RotationAngle
Telemetry.Query VehicleSimData.WheelSimData[3].RotationAngle

Telemetry.Query VehicleMovement.Mass
 */
UProperty* UReflectionUtilsFunctionLibrary::RetrieveProperty(UObject* InObject, const FString& InPath, UObject*& OutObject) 
{
	ensureAlways(InObject);

	FString ObjectString;
	FString IndexString;
	FString PropertyString;
	bool Succeded = InPath.Split(".", &ObjectString, &PropertyString);

	ensureAlways(Succeded);
	UObject *TargetObj = nullptr;
	if (ObjectString.Equals("Object", ESearchCase::IgnoreCase))
	{
		TargetObj = InObject;
	}
	else
	{
		AActor *InputActor = Cast<AActor>(InObject);

		if (InputActor == NULL)
		{
			UActorComponent *InputActorComponent = Cast<UActorComponent>(InObject);
			ensureAlways(InputActorComponent);

			//AVANTI DA QUI

			bool Succeded = InPath.Split(".", &ObjectString, &PropertyString);
			ensureAlways(Succeded);

			//check if array indexed.. if so remove [] and store index into a int
			Succeded = InPath.Split("[", &ObjectString, &IndexString);
			bool HasIndex = IndexString.Split("]", &IndexString, nullptr);

			//Get property
			FName StructName = FName(*ObjectString); //ChassisSimData
			FName PropertyName = FName(*PropertyString); //Speed
			UClass* InObjectClass = InObject->GetClass(); //VehicleSimData UCLASS
			UProperty* StructProperty = InObjectClass->FindPropertyByName(StructName);   //ChassisSimData Property
			UObject * StructObj = nullptr;

			if (!HasIndex)
			{
				StructObj =	StructProperty->ContainerPtrToValuePtr<UObject>(InObject);
			}
			else
			{
				int32 index = FCString::Atoi(*IndexString);
				StructObj = StructProperty->ContainerPtrToValuePtr<UObject>(InObject, index);
			}

			UStructProperty * StructProperty_c = Cast<UStructProperty>(StructProperty);
			UScriptStruct * ScriptStruct = StructProperty_c->Struct;

			UProperty * ChildProp = ScriptStruct->FindPropertyByName(PropertyName);
			
			if (ChildProp != nullptr) {
				OutObject = StructObj;
			}
			else {
				OutObject = nullptr;
			}

			return ChildProp;
		}
		//if  (ensureAlways(InputActor) 
		else 
		{
			TArray<UActorComponent*> CompArray;

			InputActor->GetComponents(CompArray);

			for (int32 i = 0; i < CompArray.Num(); i++)
			{
				UActorComponent *Comp = CompArray[i];
				if (ObjectString.Equals(Comp->GetName(), ESearchCase::IgnoreCase))
				{
					TargetObj = Comp;
					break;
				}
			}
		}

	}

	OutObject = TargetObj;

	if (ensureAlways(TargetObj))
	{
		UClass* ObjectClass = TargetObj->GetClass();
		FName PropertyName = FName(*PropertyString);
		bool Succeded = InPath.Split(".", &ObjectString, &PropertyString);

		if (Succeded) {
			UProperty *OutProperty = RetrieveProperty(TargetObj, PropertyString, OutObject);
			return OutProperty;
		}
		return ObjectClass->FindPropertyByName(PropertyName);
	}
	else
	{
		return nullptr;
	}
}