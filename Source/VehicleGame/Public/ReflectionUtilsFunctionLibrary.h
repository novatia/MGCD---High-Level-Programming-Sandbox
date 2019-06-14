#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ReflectionUtilsFunctionLibrary.generated.h"

UCLASS()
class VEHICLEGAME_API UReflectionUtilsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static UProperty* RetrieveProperty(UObject* InObject, const FString& InPath, UObject*& OutObject);
};