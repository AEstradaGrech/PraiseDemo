// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NeuronLayer.h"
#include "../../Enums/AI/EActivationFunction.h"
#include "../../Structs/AI/FNeuronConnection.h"
#include "NeuralNet.generated.h"

/**
 *  La clase NeuralNet se encarga principalmente de los loops entre Layers y Layer.Neurons
 *	La clase Neuron se encarga de las matematicas implicadas
 */
typedef TArray<class UNeuron*> Layer;
UCLASS()
class PRAISE_API UNeuralNet : public UObject
{
	GENERATED_BODY()
	
public:
	
	UNeuralNet();
	void Init(const TArray<int>& Topology, EActivationFunction ActivationType = EActivationFunction::TANH);	// incluye Input&OutputLayer, ademas de N HiddenLayers: ej: TArray<int> { 3, 2, 1 } = InputLayer(3 neuronas), HiddenLayer(2 neuronas), OutputLayer(1 neurona = regression != classification (output = 2 neuronas))
	void FeedForward(const TArray<float>& inputVals);			 // necesario para entrenar red. Net solo tiene que leer valores de entrada y enchufarlos en cada neurona de la capa
	void Backpropagation(const TArray<float>& targetVals);		// compensacion de errores / balanceo pesos
	void GetOutputResult(TArray<float>& resultVals) const;

private:

	bool bDidInit;
	EActivationFunction ActivationFunction;
	int LayersNum;
	TArray<Layer> NetLayers;

	float NetError;
	//para Debug
	float RecentAverageError;
	float AverageErrorSmoothingFactor;
};
