#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "FTickHandle.h"
#include "FBoolTickHandle.generated.h"

USTRUCT()
struct PRAISE_API FBoolTickHandle : public FTickHandle
{
	GENERATED_BODY()
public:

	FORCEINLINE bool GetValueToCheck() const { return Val; };
	FORCEINLINE bool GetReference() const { return Ref; };
	FORCEINLINE EComparisonType GetComparisonType() const { return Type; };

	TFunction<bool(const bool A, const bool B, EComparisonType CheckType)> BoolCheck = [this](const bool val, const bool ref, EComparisonType checkType) { return CheckBool(val, ref, checkType); };

	FORCEINLINE void SetValToCheck(bool Value) { Val = Value; };
	FORCEINLINE void SetRef(bool Reference) { Ref = Reference; };
	FORCEINLINE void SetType(EComparisonType Value) { Type = Value; };

	void Setup(bool val, bool ref, EComparisonType type) { Val = val; Ref = ref; Type = type; };
	virtual bool Check() override { return !bIsFinished && CheckBool(Val, Ref, Type); }
private:

	bool Val;
	bool Ref;
	EComparisonType Type;

	bool CheckBool(const bool Value, const bool Reference, EComparisonType CheckType)
	{
		switch (CheckType)
		{
			case(EComparisonType::NOT_EQUAL): 
				return Value != Reference;
			case(EComparisonType::EQUAL): 
			default: 
				return Value == Reference;
		}

		return false;
	};
};

