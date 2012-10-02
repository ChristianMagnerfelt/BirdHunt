#include "prediction.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <limits>
#include <cmath>
#include <map>

std::map<std::pair<ducks::EAction, ducks::EAction>, std::size_t> Prediction::observation = std::map<std::pair<ducks::EAction, ducks::EAction>, std::size_t>();

Prediction::Prediction(const ducks::CDuck & refDuck, const Matrix & initStates, const Matrix & states, const Matrix & obsStates) : 
	nextState(0), 
	nextObservation(0), 
	nextV(0), 
	nextH(0), 
	c(std::vector<float>(duck.GetSeqLength(), 0.0f)), 
	denom(0.0f), 
	numer(0.0f), 
	logProb(0.0f), 
	oldLogProb(-100000.0f), 
	maxIter(30),
	duck(refDuck), 
	init(initStates), 
	a(states), 
	b(obsStates), 
	alpha(std::vector<Matrix>(duck.GetSeqLength(), Matrix(1, a.col()))),
	beta(std::vector<Matrix>(duck.GetSeqLength(), Matrix(1, a.col()))),
	diGamma(std::vector<Matrix>(duck.GetSeqLength(), Matrix(a.row(), a.col()))), 
	gamma(std::vector<Matrix>(duck.GetSeqLength(), Matrix(1, a.col())))
{
	if(observation.empty())
	{
		observation[std::make_pair(ducks::ACTION_STOP, ducks::ACTION_STOP)] = 0;
		observation[std::make_pair(ducks::ACTION_STOP, ducks::ACTION_KEEPSPEED)] = 1;
		observation[std::make_pair(ducks::ACTION_STOP, ducks::ACTION_ACCELERATE)] = 2;
		
		observation[std::make_pair(ducks::ACTION_KEEPSPEED, ducks::ACTION_STOP)] = 3;
		observation[std::make_pair(ducks::ACTION_KEEPSPEED, ducks::ACTION_KEEPSPEED)] = 4;
		observation[std::make_pair(ducks::ACTION_KEEPSPEED, ducks::ACTION_ACCELERATE)] = 5;
		
		observation[std::make_pair(ducks::ACTION_ACCELERATE, ducks::ACTION_STOP)] = 6;
		observation[std::make_pair(ducks::ACTION_ACCELERATE, ducks::ACTION_KEEPSPEED)] = 7;
		observation[std::make_pair(ducks::ACTION_ACCELERATE, ducks::ACTION_ACCELERATE)] = 8;
	}
}

void Prediction::calculate()
{
	for(std::size_t i = 0; i < maxIter; ++i)
	{
		calculateAlpha();
		calculateBeta();
		calculateYAndDiGamma();
		reEstimate();
		calculateLogProb();
		
		if((logProb - oldLogProb) > 0.0000000001f)
		{
			oldLogProb = logProb;
			print();
		}
		else
		{
			print();
			break;
		}
	}
	makePrediction();
}
void Prediction::print() const
{
	//printDuckSequence();
	std::cout << "Init :" << std::endl;
	init.print();
	std::cout << "A :" << std::endl;
	a.print();
	std::cout << "B :" << std::endl;
	b.print();
	std::cout.precision(8);
	std::cout << "Log prob : " << logProb << std::endl;
	std::cout << "Old log prob : " << oldLogProb << std::endl;
	std::cout << "Alpha T - 1" << std::endl;
}

void Prediction::calculateAlpha()
{
	if(a.col() != a.row() || a.col() != init.col() || init.row() != 1 || a.col() != b.row())
		throw;
	
	// Compute alpha 0
	c[0] = 0.0f;
	for(std::size_t i = 0; i < a.col(); ++i)
	{
		alpha[0][0][i] = init[0][i] * b[i][getObservedState(0)];
		c[0] = c[0] + alpha[0][0][i];
	}
	
	c[0] = 1.0f/c[0];
	for(std::size_t i = 0; i < a.col(); ++i)
	{
		alpha[0][0][i] = c[0] * alpha[0][0][i];
	}

	for(std::size_t t = 1; t < duck.GetSeqLength(); ++t)
	{
		c[t] = 0.0f;
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			alpha[t][0][i] = 0;
			for(std::size_t j = 0; j < a.row(); ++j)
			{
				alpha[t][0][i] = alpha[t][0][i] + alpha[t - 1][0][j] * a[j][i];
			}
			alpha[t][0][i] = alpha[t][0][i] * b[i][getObservedState(t)];
			c[t] = c[t] + alpha[t][0][i];
		}
		
		// Scale alpha t	
		c[t] = 1.0f/c[t];
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			alpha[t][0][i] = c[t] * alpha[t][0][i];
		}
	}

}
void Prediction::calculateBeta()
{
	// Calculate beta t - 1
	for(std::size_t i = 0; i < a.col(); ++i)
	{	
		beta[duck.GetSeqLength() - 1][0][i] = c[duck.GetSeqLength() - 1];
	}

	
	// Calculate beta t - 2 to 0
	for(int t = (duck.GetSeqLength() - 2); t >= 0; --t)
	{	
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			beta[t][0][i] = 0;
			for(std::size_t j = 0; j < a.row(); ++j)
			{
				beta[t][0][i] = beta[t][0][i] + a[i][j] * b[j][getObservedState(t + 1)] * beta[t + 1][0][j];
			}
			// Scale beta with c
			beta[t][0][i] = c[t] * beta[t][0][i];				
		}
	}

}
void Prediction::calculateYAndDiGamma()
{
	for(std::size_t t = 0; t < (duck.GetSeqLength() - 1); ++t)
	{
		// Calculate denom
		denom = 0.0f;
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			for(std::size_t j = 0; j < a.row(); ++j)
			{
				denom = denom + alpha[t][0][i] * a[i][j] * b[j][getObservedState(t + 1)] * beta[t + 1][0][j];
			}
		}
		
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			gamma[t][0][i] = 0;
			for(std::size_t j = 0; j < a.row(); ++j)
			{
				diGamma[t][i][j] = (alpha[t][0][i] * a[i][j] * b[j][getObservedState(t + 1)] * beta[t + 1][0][j]) / denom;
				gamma[t][0][i] = gamma[t][0][i] + diGamma[t][i][j];
			}
		}
	}
}
void Prediction::reEstimate()
{
	// re-estimate initial states
	for(std::size_t i = 0; i < init.col(); ++i)
	{
		init[0][i] = gamma[0][0][i];
	}
	
	// re-estimate A
	for(std::size_t i = 0; i < a.row(); ++i)
	{
		for(std::size_t j = 0; j < a.col(); ++j)
		{	
			numer = 0.0f;
			denom = 0.0f;
			for(std::size_t t = 0; t < (duck.GetSeqLength() - 1); ++t)
			{
				numer = numer + diGamma[t][i][j];
				denom = denom + gamma[t][0][i];
			}
			a[i][j] = numer / denom;
		}
	}

	// re-estimate B
	for(std::size_t i = 0; i < a.row(); ++i)
	{
		for(std::size_t j = 0; j < b.col(); ++j)
		{
			numer = 0.0f;
			denom = 0.0f;
			for(std::size_t t = 0; t < (duck.GetSeqLength() - 1); ++t)
			{
				if(getObservedState(t) == j)
				{
					numer = numer + gamma[t][0][i];
				}
				denom = denom + gamma[t][0][i];
			}
			b[i][j] = numer/denom;		
		}
	}
	//std::cout << "re-estimate complete" << std::endl;
}
void Prediction::calculateLogProb()
{
	logProb = 0;
	for(std::size_t i = 0; i < duck.GetSeqLength(); ++i)
	{
		logProb = logProb + log(c[i]);
	}
	logProb = -logProb;
}

std::size_t Prediction::getObservedState(std::size_t t)
{
	ducks::CAction action = duck.GetAction(t);
	return observation[std::make_pair(action.GetVAction(), action.GetHAction())];
}

void Prediction::printDuckSequence() const
{
	for(int i = 0; i < duck.GetSeqLength(); ++i)
	{
		std::cout << "Seq : " << i << " V : " << duck.GetAction(i).GetVAction() << " H : " << duck.GetAction(i).GetHAction() << " M : " << duck.GetAction(i).GetMovement() << std::endl;
	}
}
void Prediction::makePrediction()
{
	nextState = std::max_element(alpha[duck.GetSeqLength() -1][0].begin(), alpha[duck.GetSeqLength() -1][0].end()) - alpha[duck.GetSeqLength() -1][0].begin();
	nextObservation = std::max_element(b[nextState].begin(), b[nextState].end()) - b[nextState].begin();
	std::cout << "Next state : " << nextState << std::endl;
	std::cout << "Next observation : " << nextObservation << std::endl;
	
	std::for_each(observation.begin(), observation.end(),[&, this](std::pair<const std::pair<ducks::EAction, ducks::EAction>, std::size_t> & pair)
	{
		if(pair.second == nextObservation)
		{
			nextV = pair.first.first;
			nextH = pair.first.second;
			return;
		}
	});	
}

