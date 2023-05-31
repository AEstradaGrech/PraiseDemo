#pragma once
#include "Engine/Engine.h"
#include "FOsc.generated.h"

USTRUCT()
struct PRAISE_API FOsc
{
	GENERATED_BODY()

public:

	static float Sine(float Time, float Freq, float Amplitude = 1.f, float Phase = 0.f)
	{
		float AngularFreq = 2 * PI * Freq;
		float Omega = AngularFreq * Time;

		return Amplitude * FMath::Sin(Omega + (Phase * ((PI * 2)/ 360)));
	}

	static float Square(float Time, float Freq, float Amplitude = 1.f, float Phase = 0.f)
	{
		float Sine = FOsc::Sine(Time, Freq, 1, Phase);

		return  (Sine > 0 ? 1.f : -1.f) * Amplitude;
	}

	static float Triangle(float Time, float Freq, float Amplitude = 1.f, float Phase = 0.f)
	{
		return (2*Amplitude / PI) * FMath::Asin(FOsc::Sine(Time, Freq, 1, Phase));
	}

	static float Sawtooth(float Time, float Freq, float Amplitude = 1.f, float Phase = 0.f)
	{
		return (2 * Amplitude / PI) * (Freq * PI * FMath::Fmod(Time, 1 / Freq) - (PI / 2.f));
	}

};