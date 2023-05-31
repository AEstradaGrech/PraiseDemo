// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NeuralNet.h"								// para incluir typedef Layer
#include "../../Structs/AI/FNeuronConnection.h"
#include "../../Enums/AI/EActivationFunction.h"
#include "Neuron.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UNeuron : public UObject
{
	GENERATED_BODY()
	
public:
	UNeuron();
	void Init(/*PrevLayer& */int ID, int OutputConnections, EActivationFunction ActFunc);


	FORCEINLINE int GetID() const { return NeuronID; }
	FORCEINLINE float GetOutputValue() const { return Output; }
	FORCEINLINE void SetOutputValue(float Val) { Output = Val; }

	void FeedForward(const Layer& prevLayer);
	void CalculateOutputGradient(float TargetValue);
	void CalculateHiddenLayerGradient(const Layer& nextLayer);
	void UpdateInputWeights(Layer& prevLayer);
	static float ApplyTransferFunction(float x, EActivationFunction FuncType = EActivationFunction::TANH);
	static float ApplyTransFuncDerivative(float x, EActivationFunction FuncType = EActivationFunction::TANH);

private:
	int NeuronID; // ID para cada LAYER ( no ID en RED. Layer1.Neurona1, Layer2.Neurona1 etc
	EActivationFunction ActivationFunction;
	bool bDidInit;

	float Output;
	float Gradient;
	
	// el output de cada neurona alimenta el input DE CADA NEURONA DE LA SIGUIENTE CAPA
	// para evitar conexiones raras / feedback (no deseado) cada neurona contiene un array
	// con el numero de neuronas de la siguiente capa (en vez de mapear Output a Neurona_X, Output a Neurona_X1...)
	// como tambien es necesario Deltaweight (necesario para calculo de 'momentum') se organiza el output con el struct
	// y se pasa el numero de conexiones (en principio desconocido por Neurona) en la Inicializacion
	TArray<FNeuronConnection> OutputWeights; // incluye output para las neuronas de la siguiente capa y tambien DeltaWeight

	float SumDOW(const Layer& nextLayer) const;
};

// overall net learning rate [0..1]. Todas las neuronas usan el mismo valor asi que puede ser static
	//    - Eta=0   - slow learner
	//	  - Eta=0.2 - medium learner
	//    - Eta=1.f - reckless learner
static float Eta = 0.15f;
// multiplier of last neuron weight change (momentum) [0..1] Todas las neuronas usan el mismo valor asi que puede ser static
//	  - Momentum=0    - no momentum
//    - Momentum=0.5f - moderate momentum
static float Momentum = 0.5f;
