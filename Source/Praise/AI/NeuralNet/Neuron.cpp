// Fill out your copyright notice in the Description page of Project Settings.


#include "Neuron.h"
#include <cmath>

UNeuron::UNeuron()
{
	bDidInit = false;
}

void UNeuron::Init(int ID, int OutputConnections, EActivationFunction ActFunc)
{
	if (ID < 0) return;

	if (OutputConnections <= 0) return;

	ActivationFunction = ActFunc;

	NeuronID = ID;

	for (int i = 0; i < OutputConnections; i++)
	{
		OutputWeights.Add(FNeuronConnection());
		OutputWeights[i].Weight = FMath::Rand() / RAND_MAX; // se inicializan con un peso random. esto hace que en los primeros pases / entrenamiento el resultado pueda acercarse a target, pero es casualidad y se irá corrigiendo en los subsiguientes pases
	}

	bDidInit = true;
}

void UNeuron::FeedForward(const Layer& prevLayer)
{
	float sum = 0.f;
	//sumar pesos de las neuronas de la capa anterior (que son el input de cada neurona de la capa siguiente).
	// incluye la neurona de BIAS
	for (int i = 0; i < prevLayer.Num(); i++)
		sum += prevLayer[i]->GetOutputValue() * prevLayer[i]->OutputWeights[NeuronID].Weight; // OutputNeurona = SumatorioPrevLayerNeurons * NeuronWeight indexado a esta neurona
	
	// ejecutar funcion de activacion
	Output = UNeuron::ApplyTransferFunction(sum, ActivationFunction);
}

void UNeuron::CalculateOutputGradient(float TargetValue)
{
	float delta = TargetValue - Output;

	Gradient = delta * ApplyTransFuncDerivative(Output, ActivationFunction);
}

// delta error en Hidden layer se calcula con la suma de los derivativos de los wights de la siguiente capa (no hay TargetValue con el que comparar)
// de eso se encarga la funcion sumDOW (que coño significa DOW? DerivativeOfWeigths?)
void UNeuron::CalculateHiddenLayerGradient(const Layer& nextLayer)
{
	float DOW = SumDOW(nextLayer);

	Gradient = DOW * ApplyTransFuncDerivative(Output);
}

void UNeuron::UpdateInputWeights(Layer& prevLayer)
{
	// The weights to update are in the Connection struct of the prevLayer neurons
	for (int i = 0; i < prevLayer.Num() - 1; i++)
	{
		UNeuron* PrevNeuron = prevLayer[i];

		float oldDeltaWeight = PrevNeuron->OutputWeights[i].DeltaWeight;
		//composed of two main components
		// expresa el cambio  en weight entre neurona previa y actual. Individual input, magnified by the gradient and train rate
		// Eta = learning rate
		// MomentumFactor = amount of change since last train sample
		float newDeltaWeight =
			Eta *										// learning rate multiplied by 
			PrevNeuron->GetOutputValue() * Gradient +	// previousNeuronOutput Times the current neuron gradient
			Momentum *										// plus a fraction of previous DeltaWeight. Is multiplier of overall learning rate since last train sample (como feedback fraction en amplificadores)
			oldDeltaWeight;								// 'so it keeps moving in the same direction'... entiendo que se trata de 'informar' a la neurona de los resultados de la training sample anterior para condicionar el resultado y que vaya en la misma linea
	
		// actualizar neurona / aplicar feedback
		PrevNeuron->OutputWeights[i].DeltaWeight = newDeltaWeight;
		PrevNeuron->OutputWeights[i].Weight += newDeltaWeight;
	}
}

float UNeuron::ApplyTransferFunction(float x, EActivationFunction FuncType)
{
	switch (FuncType)
	{
		case(EActivationFunction::TANH):
			return std::tanh(x);
		case(EActivationFunction::SIGMOIDAL):
		case(EActivationFunction::ReLU):
		case(EActivationFunction::LEAKY_ReLU):
		default:
			return 0.f;
	}

	return 0.0f;
}

float UNeuron::ApplyTransFuncDerivative(float x, EActivationFunction FuncType)
{
	switch (FuncType)
	{
		case(EActivationFunction::TANH):
			return 1 - x * x; // aproximacion valida para derivative de Tanh func (no es realmente la formula del derivative -> [1-tanh^2*x] -->  1 - std::tanh(x) * std::tanh(x))
		case(EActivationFunction::SIGMOIDAL):
		case(EActivationFunction::ReLU):
		case(EActivationFunction::LEAKY_ReLU):
		default:
			return 0.f;
	}

	return 0.0f;
}

float UNeuron::SumDOW(const Layer& nextLayer) const
{
	float sum = 0.f;

	for (int i = 0; i < nextLayer.Num() - 1; i++) // se exluye BIAS
		sum += OutputWeights[i].Weight * nextLayer[i]->Gradient;

	return sum;
}

