#include "VehicleGame.h"
#include "ReflectionUtilsFunctionLibrary.h"

UProperty* UReflectionUtilsFunctionLibrary::RetrieveProperty(UObject* InObject, const FString& InPath, UObject*& OutObject) 
{
	ensureAlways(InObject);

	FString ObjectString;
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

		if (ensureAlways(InputActor))
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
		return ObjectClass->FindPropertyByName(PropertyName);
	}
	else
	{
		return nullptr;
	}
}