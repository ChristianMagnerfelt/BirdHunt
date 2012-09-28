#include "prediction.h"

void Prediction::calculateAlpha()
{
	if(initStates.col() != obsStates.row())
		throw;
		
	// Create alpha 
	alpha.resize(duck.GetSeqLength(), Matrix(1, initStates.col()));
	
	// Compute alpha 0
	for(std::size_t i = 0; i < initStates.col(); ++i)
	{
		alpha[0][0][i] = initStates[0][i] * obsStates[i][getObservedState(0)];
		c += alpha[0][0][i];
	}
	
	// Scale alpha 0
	c = 1/c;
	for(std::size_t i = 0; i < alpha[0].col(); ++i)
	{
		alpha[0][0][i] = c * alpha[0][0][i];
	}

	// Compute alpha t
	for(std::size_t t = 1; t < duck.GetSeqLength(); ++t)
	{
		c = 0;
		alpha.emplace_back(Matrix(1, initStates.col()));
		
		for(std::size_t i = 0; i < initStates.row(); ++i)
		{
			for(std::size_t j = 0; j < initStates.col(); ++j)
			{
				alpha[t][0][i] = alpha[t][0][i] + alpha[t - 1][0][j] * initStates[j][i];
			}
			alpha[t][0][i] = alpha[t][0][i] * obsStates[i][getObservedState(t)];
			c += alpha[t][0][i];
		}
		
		// Scale alpha t
		c = 1/c;
		for(std::size_t i = 0; i < initStates.row(); ++i)
		{
			alpha[t][0][i] = c * alpha[t][0][i];
		}
	}
}
void Prediction::calculateBeta()
{
	beta.resize(duck.GetSeqLength(), Matrix(1, initStates.col()));
	for(std::size_t i = 0; i < initStates.col(); ++i)
	{	
		beta[beta.size() - 1][0][i] = c;
	}
	for(int t = duck.GetSeqLength() - 2; t >= 0; --t)
	{	
		for(std::size_t i = 0; i < initStates.col(); ++i)
		{
			beta[t][0][i] = 0;
			for(std::size_t j = 0; j < initStates.col(); ++j)
			{
				beta[t][0][i] = beta[t][0][i] + initStates[j][i] * obsStates[j][getObservedState(t)] * beta[t - 1][0][j]; 
			}
			// Scale beta with c
			beta[t][0][i] = c * beta[t][0][i];					
		}
	}
}
void Prediction::calculateYAndDiGamma()
{
	for(std::size_t t = 0; t < duck.GetSeqLength() - 1; ++t)
	{
		denom = 0;
		for(std::size_t i = 0; i < initStates.col(); ++i)
		{
			for(std::size_t j = 0; j < initStates.row(); ++j)
			{
				denom = denom + alpha[t][0][i] * initStates[j][i] * obsStates[j][getObservedState(t + 1)] * beta[t + 1][0][j];
			}
		}
	}
}
std::size_t Prediction::getObservedState(std::size_t t)
{

}	
