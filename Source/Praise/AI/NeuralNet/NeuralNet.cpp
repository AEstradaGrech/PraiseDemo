// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNet.h"
#include "Neuron.h"
#include "../../Structs/CommonUtility/FLogger.h"

UNeuralNet::UNeuralNet()
{
	bDidInit = false;
	LayersNum = 0;
	NetError = 0.f;
}

void UNeuralNet::Init(const TArray<int>& Topology, EActivationFunction ActivationType)
{
	if (Topology.Num() <= 0) return;

	LayersNum = Topology.Num();

	ActivationFunction = ActivationType;

	for (int i = 0; i < LayersNum; i++)
	{
		if (Topology[i] <= 0) return;

		NetLayers.Add(Layer());

		for (int j = 0; j <= Topology[i]; i++) // <= para añadir la neurona de BIAS
		{
			UNeuron* Neuron = NewObject<UNeuron>();

			int NeuronOutputs = i == LayersNum - 1 ? 0 : Topology[i + 1]; // la ultima capa no tiene outputs
			
			Neuron->Init(i, NeuronOutputs, ActivationFunction);

			NetLayers[i].Add(Neuron);

			FLogger::LogTrace(__FUNCTION__ + FString(" :: LAYER --> ") + FString::FromInt(i) + FString(" :: CREATED NEURON Nº :") + FString::FromInt(j) + FString(" :: NeuronID --> ") + FString::FromInt(Neuron->GetID()));
		}

		//En la neurona de BIAS se hardcodea el weight a 1
		NetLayers[i].Last()->SetOutputValue(1.f);
	}

	bDidInit = true;
}

void UNeuralNet::FeedForward(const TArray<float>& inputVals)
{
	// si numero de inputs no es igual a inputs de red menos neurona BIAS no se puede hacer FeedFWD
	if (inputVals.Num() != NetLayers[0].Num() - 1) return; 

	// asignar input values a input neurons
	for (int i = 0; i < inputVals.Num(); i++)
		NetLayers[0][i]->SetOutputValue(inputVals[i]);
	
	// Feed forward empezando por la primera HiddenLayer (i = 1)
	for (int layerIdx = 1; layerIdx < LayersNum; layerIdx++)
	{
		Layer& previousLayer = NetLayers[layerIdx - 1];

		for (int neuronIdx = 0; neuronIdx < NetLayers[layerIdx].Num() - 1; neuronIdx++) // se omite neurona de BIAS
			NetLayers[layerIdx][neuronIdx]->FeedForward(previousLayer);
	}

}

void UNeuralNet::Backpropagation(const TArray<float>& targetVals)
{
	// calcular overall net error (RMS output neurons error)

	Layer& outputLayer = NetLayers.Last();

	float overallError = 0.f;

	for (int i = 0; outputLayer.Num() - 1; i++)
	{
		float delta = targetVals[i] - outputLayer[i]->GetOutputValue();

		overallError += delta * delta;
	}

	overallError /= outputLayer.Num() - 1; // valor medio del error en salida sin contar neurona de BIAS

	NetError = FMath::Square(overallError); // valor RMS
	// para debug
	RecentAverageError = RecentAverageError * AverageErrorSmoothingFactor + NetError / (AverageErrorSmoothingFactor + 1);

	// calcular gradiente output layer
	for (int i = 0; i < outputLayer.Num() - 1; i++)
		outputLayer[i]->CalculateOutputGradient(targetVals[i]);

	// calcular gradientes hidden layers
	for (int h = LayersNum - 2; h > 0; h--)
	{
		Layer& hiddenLayer = NetLayers[h];
		Layer& nextHiddenLayer = NetLayers[h + 1];

		for (int neuron = 0; neuron < hiddenLayer.Num(); neuron++)
			hiddenLayer[neuron]->CalculateHiddenLayerGradient(nextHiddenLayer);
	}

	// aplicar feedback / update connection weights para cada layer desde HiddenLayer-1 a OutputLayer
	for (int layer = NetLayers.Num() - 1; layer > 0; layer--) // se exluyer InputLayer
	{
		Layer& currentLayer = NetLayers[layer];
		Layer& previous = NetLayers[layer - 1];

		for (int neuron = 0; neuron < currentLayer.Num() - 1; neuron++)
			currentLayer[neuron]->UpdateInputWeights(previous); 
	}
}

void UNeuralNet::GetOutputResult(TArray<float>& resultVals) const
{
	resultVals.Empty();

	for (int i = 0; i < NetLayers.Last().Num() - 1; i++)		// se excluye neurona de BIAS
		resultVals.Add(NetLayers.Last()[i]->GetOutputValue());
	
	FLogger::LogTrace(__FUNCTION__ + FString(" :: NET AVERAGE ERROR : ") + FString::SanitizeFloat(RecentAverageError));
}
