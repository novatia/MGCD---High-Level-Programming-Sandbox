#include "VehicleGame.h"

#include "TelemetryComponent.h"
#include "TelemetryVehiclePawn.h"

#include "Engine/Canvas.h"
#include "Engine/Font.h"

#include "ReflectionUtilsFunctionLibrary.h"

#if !UE_BUILD_SHIPPING

static void UpdateTelemetryQuery(const TArray<FString>& InArgs, UWorld* InWorld)
{
	if (InArgs.Num() == 0)
	{
		return;
	}

	const FString& TelemetryQuery = InArgs[0];

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(InWorld, 0);
	if (PlayerController)
	{
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
		{
			ATelemetryVehiclePawn* TelemetryVehiclePawn = Cast<ATelemetryVehiclePawn>(PlayerPawn);
			if (TelemetryVehiclePawn && TelemetryVehiclePawn->TelemetryComponent)
			{
				UTelemetryComponent* TelemetryComponent = TelemetryVehiclePawn->TelemetryComponent;
				TelemetryComponent->SetTelemetryQuery(TelemetryQuery);
			}
		}
	}
}

static void UpdateTelemetryGraphRange(const TArray<FString>& InArgs, UWorld* InWorld)
{
	if (InArgs.Num() == 0)
	{
		return;
	}

	FVector2D TelemetryGraphRange;
	TelemetryGraphRange.InitFromString(InArgs[0]);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(InWorld, 0);
	if (PlayerController)
	{
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
		{
			ATelemetryVehiclePawn* TelemetryVehiclePawn = Cast<ATelemetryVehiclePawn>(PlayerPawn);
			if (TelemetryVehiclePawn && TelemetryVehiclePawn->TelemetryComponent)
			{
				UTelemetryComponent* TelemetryComponent = TelemetryVehiclePawn->TelemetryComponent;
				TelemetryComponent->SetTelemetryGraphRange(TelemetryGraphRange);
			}
		}
	}
}

static FAutoConsoleCommandWithWorldAndArgs UpdateTelemetryQueryCommand(
	TEXT("Telemetry.Query"),
	TEXT("Telemetry query"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(UpdateTelemetryQuery)
);

static FAutoConsoleCommandWithWorldAndArgs UpdateTelemetryGraphRangeCommand(
	TEXT("Telemetry.Range"),
	TEXT("Telemetry graph range"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(UpdateTelemetryGraphRange)
);

// See: https://docs.unrealengine.com/en-US/Programming/Development/Tools/ConsoleManager/index.html

#endif // !UE_BUILD_SHIPPING

UTelemetryComponent::UTelemetryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTelemetryComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateTelemetryTargets();
}

void UTelemetryComponent::SetTelemetryQuery(const FString& InTelemetryQuery)
{
	TelemetryList = InTelemetryQuery;

	UpdateTelemetryTargets();
}

void UTelemetryComponent::SetTelemetryGraphRange(FVector2D InRange)
{
	TelemetryRange = InRange;
}

void UTelemetryComponent::UpdateTelemetryTargets()
{
	TelemetryTargets.Reset();

	if (TelemetryList.Len() > 0)
	{
		TelemetryList.ParseIntoArray(TelemetryArray, TEXT(","));

		for (FString& Telemetry : TelemetryArray)
		{
			UObject* Object = nullptr;
			UProperty* Property = UReflectionUtilsFunctionLibrary::RetrieveProperty(GetOwner(), Telemetry, Object);

			TelemetryTarget Target;
			Target.TargetObject = Object;
			Target.TargetProperty = Property;

			TelemetryTargets.Add(Target);
		}
	}
}

void DrawLabel(UCanvas* InCanvas, const FString& InLabel, const FColor& InColor, float InX, float InY, float InMaxWidth, float& OutX, float& OutY)
{
	UFont* Font = GEngine->GetSmallFont();

	InCanvas->SetDrawColor(InColor);

	float XL, YL;
	InCanvas->TextSize(Font, InLabel, XL, YL);

	float ScaleX = 1.f;
	if (XL > InMaxWidth)
	{
		ScaleX = InMaxWidth / XL;
	}

	InCanvas->DrawText(Font, InLabel, InX, InY, ScaleX);

	OutX = InX;
	OutY = InY + YL;
}

void DrawGraph(UCanvas* InCanvas, const TArray<float>& Values, const FVector2D& Range, float InCurValue, const FLinearColor& InBackgroundColor, const FLinearColor& InColor, float InX, float InY, float InGraphWidth, float InGraphHeight, bool InDrawXAxis, bool InDrawYAxis, float& OutX, float& OutY)
{
	FCanvasTileItem TileItem(FVector2D(InX, InY), GWhiteTexture, FVector2D(InGraphWidth, InGraphHeight), InBackgroundColor);
	TileItem.BlendMode = SE_BLEND_Translucent;
	InCanvas->DrawItem(TileItem);

	InCanvas->SetDrawColor(FColor(0, 0, 32, 92));

	if (InDrawXAxis)
	{
		const float Y = FMath::GetMappedRangeValueClamped(Range, FVector2D(1.f, 0.f), 0.f);

		FCanvasLineItem XAxis(FVector2D(InX, InY + Y * InGraphHeight), FVector2D(InX + InGraphWidth, InY + Y * InGraphHeight));
		XAxis.SetColor(FLinearColor(.25f, .25f, .25f, .25f));
		XAxis.Draw(InCanvas->Canvas);
	}

	InCanvas->SetDrawColor(FColor(0, 32, 0, 128));

	for (int32 ValueIndex = 1; ValueIndex < Values.Num(); ++ValueIndex)
	{
		const float X1 = static_cast<float>(ValueIndex - 1) / Values.Num();
		const float Y1 = FMath::GetMappedRangeValueClamped(Range, FVector2D(1.f, 0.f), Values[ValueIndex - 1]);
		const float X2 = static_cast<float>(ValueIndex) / Values.Num();
		const float Y2 = FMath::GetMappedRangeValueClamped(Range, FVector2D(1.f, 0.f), Values[ValueIndex]);

		if (InDrawYAxis && X1 <= 0.f && X2 > 0.f)
		{
			FCanvasLineItem YAxis(FVector2D(InX + X1 * InGraphWidth, InY), FVector2D(InX + X1 * InGraphWidth, InY + InGraphHeight));
			YAxis.SetColor(FLinearColor(.25f, .25f, .25f, .25f));
			YAxis.Draw(InCanvas->Canvas);
		}

		FCanvasLineItem LineItem(FVector2D(InX + X1 * InGraphWidth, InY + Y1 * InGraphHeight), FVector2D(InX + X2 * InGraphWidth, InY + Y2 * InGraphHeight));
		LineItem.SetColor(InColor);
		LineItem.Draw(InCanvas->Canvas);

		if (InCurValue >= X1 && (InCurValue < X2 || ValueIndex == Values.Num() - 1))
		{
			FCanvasLineItem CurrentPositionLine(FVector2D(InX + X1 * InGraphWidth, InY), FVector2D(InX + X1 * InGraphWidth, InY + InGraphHeight));
			CurrentPositionLine.SetColor(FLinearColor(1.f, 1.f, 1.f, 0.f));
			CurrentPositionLine.Draw(InCanvas->Canvas);
		}
	}

	OutX = InX;
	OutY = InY + InGraphHeight;
}

void UTelemetryComponent::DrawTelemetry(UCanvas* Canvas, float& YL, float& YPos)
{
	static const float GraphW = 300.0f;
	static const float GraphH = 150.0f;

	static const int32 NumValues = 128;

	static const TArray<FColor> Colors = { FColor::Yellow, FColor::Red, FColor::Blue, FColor::Cyan, FColor::Magenta, FColor::Orange, FColor::Purple, FColor::White };
	static const int32 NumColors = 8;

	static const TArray<float> EmptyArray;

	static const float XPos = 4.f;
	static const float YSep = 4.f;

	static const FLinearColor BackgroundColor = FLinearColor(0.0f, 0.125f, 0.0f, 0.25f);
	static const FLinearColor NoColor = FLinearColor(0.0f, 0.f, 0.0f, 0.f);

	UFont* SmallFont = GEngine->GetSmallFont();

	float CurrX = XPos;
	float CurrY = YPos + 16.f;

	// Read Console and parse command.

	TArray<float> TelemetryValues;

	void* Object = nullptr;

	for (TelemetryTarget& Target : TelemetryTargets)
	{
		float Value = 0.f;

		if (Target.TargetProperty && Target.TargetObject)
		{
			UFloatProperty* FloatProperty = Cast<UFloatProperty>(Target.TargetProperty);
			if (FloatProperty)
			{
				Value = FloatProperty->GetPropertyValue_InContainer(Target.TargetObject);
			}
			else
			{
				UBoolProperty* BoolProperty = Cast<UBoolProperty>(Target.TargetProperty);
				if (BoolProperty)
				{
					Value = BoolProperty->GetPropertyValue_InContainer(Target.TargetObject) ? 1.f : 0.f;
				}
			}
		}

		TelemetryValues.Add(Value);
	}

	// Draw title.

	{
		FString Label = "TELEMETRY";

		float OutX;
		DrawLabel(Canvas, Label, FColor::White, CurrX, CurrY, GraphW, OutX, CurrY);
	}

	CurrY += YSep;

	// Draw labels.

	int32 TelemetryIdx = 0;

	for (float Value : TelemetryValues)
	{
		FString Label = TelemetryArray[TelemetryIdx];
		Label = Label.Append(" : ");
		Label = Label.Append(FString::Printf(TEXT("%0.3f"), Value));

		FColor CurrentColor = Colors[TelemetryIdx % NumColors];

		float OutX;
		DrawLabel(Canvas, Label, CurrentColor, CurrX, CurrY, GraphW, OutX, CurrY);

		++TelemetryIdx;
	}

	CurrY += YSep;

	// Print graph range.

	{
		FString RangeLabel = FString::Printf(TEXT("[Min: %0.3f ; Max : %0.3f]"), TelemetryRange.X, TelemetryRange.Y);

		float OutX;
		DrawLabel(Canvas, RangeLabel, FColor::Green, CurrX, CurrY, GraphW, OutX, CurrY);
	}

	CurrY += YSep;

	// Draw background and graphs.

	{
		float OutX;
		float OutY;
		DrawGraph(Canvas, EmptyArray, TelemetryRange, 0.f, BackgroundColor, NoColor, CurrX, CurrY, GraphW, GraphH, false, false, OutX, OutY);
	}

	TelemetryIdx = 0;

	for (float Value : TelemetryValues)
	{
		if (TelemetryGraphValues.Num() <= TelemetryIdx)
		{
			TelemetryGraphValues.SetNum(TelemetryIdx + 1);
		}

		TelemetryGraphValues[TelemetryIdx].SetNum(NumValues);
		TelemetryGraphValues[TelemetryIdx][TelemetryGraphIndex] = Value;

		const TArray<float>& Values = TelemetryGraphValues[TelemetryIdx];

		FColor CurrentColor = Colors[TelemetryIdx % NumColors];

		float OutX;
		float OutY;
		DrawGraph(Canvas, Values, TelemetryRange, (float)TelemetryGraphIndex / NumValues, NoColor, CurrentColor, CurrX, CurrY, GraphW, GraphH, true, false, OutX, OutY);

		++TelemetryIdx;
	}

	TelemetryGraphIndex = (TelemetryGraphIndex + 1) % NumValues;
}