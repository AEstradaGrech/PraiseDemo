#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "FTickHandle.h"
#include "FFloatTickHandle.generated.h"

USTRUCT()
struct PRAISE_API FFloatTickHandle : public FTickHandle
{
	GENERATED_BODY()
public:

	FORCEINLINE float GetValueToCheck() const { return Val; };
	FORCEINLINE float GetReference() const { return Ref; };
	FORCEINLINE EComparisonType GetComparisonType() const { return Type; };

	TFunction<bool(const float A, const float B, EComparisonType CheckType)> FloatCheck = [this](const float val, const float ref, EComparisonType checkType) { return CheckFloat(val, ref, checkType); };

	FORCEINLINE void SetValToCheck(float Value) { Val = Value; };
	FORCEINLINE void SetRef(float Reference) { Ref = Reference; };
	FORCEINLINE void SetType(EComparisonType Value) { Type = Value; };

	void Setup(float val, float ref, EComparisonType type) { Val = val; Ref = ref; Type = type; };
	virtual bool Check() override { return !bIsFinished && CheckFloat(Val, Ref, Type); }
private:

	float Val;
	float Ref;
	EComparisonType Type;

	bool CheckFloat(const float Value, const float Reference, EComparisonType CheckType)
	{
		switch (CheckType)
		{
			case(EComparisonType::LESS): return Value < Reference;
			case(EComparisonType::MORE): return Value > Reference;
			case(EComparisonType::LESS_OR_EQUAL): return Value <= Reference;
			case(EComparisonType::MORE_OR_EQUAL): return Value >= Reference;
			case(EComparisonType::NOT_EQUAL): return Value != Reference;
			case(EComparisonType::EQUAL): default: return Value == Reference;
		}

		return false;
	};
};

