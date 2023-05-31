#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "FTickHandle.generated.h"

UENUM()
enum class EComparisonType : uint8
{
	EQUAL,
	LESS,
	MORE,
	LESS_OR_EQUAL,
	MORE_OR_EQUAL,
	NOT_EQUAL
};


//https://forums.unrealengine.com/t/how-can-i-convert-a-delegate-to-tfunction/75507

USTRUCT()
struct PRAISE_API FTickHandle
{
	GENERATED_BODY()
public:
	
	virtual ~FTickHandle() {};
	TFunction<bool()> FunctionCheck;
	TFunction<void(const float DeltaTime)> HandlingFunction;
	TFunction<void(FTickHandle& Self)>OnCheckedCallback;

	FORCEINLINE int GetHandleKey() const { return Key; };
	FORCEINLINE FString GetHandleName() const { return Name; };
	FORCEINLINE bool IsFinished() const { return bIsFinished; };

	FORCEINLINE void SetKey(int key) { Key = key; };
	FORCEINLINE void SetName(FString name) { Name = name; };
	FORCEINLINE void SetFinished() { bIsFinished = true; };
protected:

	int Key;
	FString Name;
	bool bIsFinished = false;

public:

	virtual bool Check() { return !bIsFinished && FunctionCheck(); };

};

