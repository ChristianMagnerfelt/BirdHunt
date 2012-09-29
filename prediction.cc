#include "prediction.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <limits>
#include <cmath>

std::vector<Prediction::CombinedAction> Prediction::combAction = std::vector<Prediction::CombinedAction>();

Prediction::Prediction(const ducks::CDuck & refDuck, const Matrix & initStates, const Matrix & states, const Matrix & obsStates) : 
	c(std::vector<float>(duck.GetSeqLength(), 0.0f)), 
	denom(0.0f), numer(0.0f), logProb(0.0f), oldLogProb(std::numeric_limits<float>::min()), maxIter(5),
	duck(refDuck), init(initStates), a(states), b(obsStates), 
	alpha(std::vector<Matrix>(duck.GetSeqLength(), Matrix(1, a.col()))),
	beta(std::vector<Matrix>(duck.GetSeqLength(), Matrix(1, a.col()))),
	diGamma(std::vector<Matrix>(duck.GetSeqLength(), Matrix(a.row(), a.col()))), 
	gamma(std::vector<Matrix>(duck.GetSeqLength(), Matrix(1, a.col())))
{
	if(combAction.empty())
	{
		// Migration
		combAction.emplace_back(CombinedAction(ducks::ACTION_STOP, ducks::ACTION_KEEPSPEED, ducks::MOVE_EAST, StableEast));
		combAction.emplace_back(CombinedAction(ducks::ACTION_STOP, ducks::ACTION_KEEPSPEED, ducks::MOVE_WEST, StableWest));
		// Quacking
		combAction.emplace_back(CombinedAction(ducks::ACTION_KEEPSPEED, ducks::ACTION_KEEPSPEED, (ducks::EMovement)(ducks::MOVE_UP&ducks::MOVE_EAST), SlowUpEast));
		combAction.emplace_back(CombinedAction(ducks::ACTION_KEEPSPEED, ducks::ACTION_KEEPSPEED, (ducks::EMovement)(ducks::MOVE_UP&ducks::MOVE_WEST), SlowUpWest));
		combAction.emplace_back(CombinedAction(ducks::ACTION_KEEPSPEED, ducks::ACTION_KEEPSPEED, (ducks::EMovement)(ducks::MOVE_DOWN&ducks::MOVE_EAST), SlowDownEast));
		combAction.emplace_back(CombinedAction(ducks::ACTION_KEEPSPEED, ducks::ACTION_KEEPSPEED, (ducks::EMovement)(ducks::MOVE_DOWN&ducks::MOVE_WEST), SlowDownWest));
		// Panicking
		combAction.emplace_back(CombinedAction(ducks::ACTION_ACCELERATE, ducks::ACTION_ACCELERATE, (ducks::EMovement)(ducks::MOVE_UP&ducks::MOVE_EAST), FastUpEast));
		combAction.emplace_back(CombinedAction(ducks::ACTION_ACCELERATE, ducks::ACTION_ACCELERATE, (ducks::EMovement)(ducks::MOVE_UP&ducks::MOVE_WEST), FastUpWest));
		combAction.emplace_back(CombinedAction(ducks::ACTION_ACCELERATE, ducks::ACTION_ACCELERATE, (ducks::EMovement)(ducks::MOVE_DOWN&ducks::MOVE_EAST), FastDownEast));
		combAction.emplace_back(CombinedAction(ducks::ACTION_ACCELERATE, ducks::ACTION_ACCELERATE, (ducks::EMovement)(ducks::MOVE_DOWN&ducks::MOVE_WEST), FastDownWest));
		// Feigning death
		combAction.emplace_back(CombinedAction(ducks::ACTION_ACCELERATE, ducks::ACTION_KEEPSPEED, (ducks::EMovement)(ducks::MOVE_DOWN&ducks::MOVE_EAST), FallEast));
		combAction.emplace_back(CombinedAction(ducks::ACTION_ACCELERATE, ducks::ACTION_KEEPSPEED, (ducks::EMovement)(ducks::MOVE_DOWN&ducks::MOVE_WEST), FallWest));
		// Stopped
		combAction.emplace_back(CombinedAction(ducks::ACTION_STOP, ducks::ACTION_STOP, ducks::BIRD_STOPPED, Stopped));
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
		print();
		oldLogProb = logProb;
	}
}
void Prediction::print() const
{
	std::cout << "Init :" << std::endl;
	init.print();
	std::cout << "A :" << std::endl;
	a.print();
	std::cout << "B :" << std::endl;
	b.print();
	std::cout << "Log prob : " << logProb << std::endl;
	std::cout << "Old log prob : " << oldLogProb << std::endl;
}
void Prediction::calculateAlpha()
{
	if(a.col() != a.row() || a.col() != init.col() || init.row() != 1 || a.col() != b.row())
		throw;
	
	// Compute alpha 0
	c[0] = 0;
	std::cout << "Compute alpha 0" << std::endl;
	for(std::size_t i = 0; i < init.col(); ++i)
	{
		alpha[0][0][i] = init[0][i] * b[i][getObservedState(0)];	
		c[0] = c[0] + alpha[0][0][i];
	}
	
	// Scale alpha 0
	std::cout << "Scale alpha 0" << std::endl;
	c[0] = 1.0f/c[0];
	for(std::size_t i = 0; i < alpha[0].col(); ++i)
	{
		alpha[0][0][i] = c[0] * alpha[0][0][i];
	}

	// Compute alpha t
	std::cout << "Compute alpha t" << std::endl;
	for(std::size_t t = 1; t < duck.GetSeqLength(); ++t)
	{
		c[t] = 0.0f;
		
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			for(std::size_t j = 0; j < a.row(); ++j)
			{
				alpha[t][0][i] = alpha[t][0][i] + alpha[t - 1][0][j] * a[j][i];
			}
			alpha[t][0][i] = alpha[t][0][i] * b[i][getObservedState(t)];
			c[t] += alpha[t][0][i];
		}
		
		// Scale alpha t
		
		c[t] = 1.0f/c[t];
		for(std::size_t i = 0; i < a.row(); ++i)
		{
			alpha[t][0][i] = c[t] * alpha[t][0][i];
		}
	}
	std::cout << "Completed alpha" << std::endl;
}
void Prediction::calculateBeta()
{
	// Calculate beta t - 1
	std::cout << "Calculate beta t - 1" << std::endl;
	for(std::size_t i = 0; i < a.col(); ++i)
	{	
		beta[beta.size() - 1][0][i] = c[beta.size() - 1];
	}

	// Calculate beta t - 2 to 0
	std::cout << "Calculate beta t - 2 to 0" << std::endl;
	for(int t = duck.GetSeqLength() - 2; t >= 0; --t)
	{	
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			beta[t][0][i] = 0;
			for(std::size_t j = 0; j < a.row(); ++j)
			{
				beta[t][0][i] = beta[t][0][i] + a[j][i] * b[j][getObservedState(t + 1)] * beta[t + 1][0][j];
			}
			// Scale beta with c
			beta[t][0][i] = c[t] * beta[t][0][i];				
		}
	}
	std::cout << "Beta complete" << std::endl;
}
void Prediction::calculateYAndDiGamma()
{
	for(std::size_t t = 0; t < duck.GetSeqLength() - 2; ++t)
	{
		// Calculate denom
		denom = 0.0f;
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			for(std::size_t j = 0; j < a.row(); ++j)
			{
				denom = denom + alpha[t][0][i] * a[j][i] * b[j][getObservedState(t + 1)] * beta[t + 1][0][j];
			}
		}
		
		
		for(std::size_t i = 0; i < a.col(); ++i)
		{
			gamma[t][0][i] = 0;
			for(std::size_t j = 0; j < a.row(); ++j)
			{
				diGamma[t][j][i] = (alpha[t][0][i] * a[j][i] * b[j][getObservedState(t + 1)] * beta[t + 1][0][j]) / denom;
				gamma[t][0][i] = gamma[t][0][i] + diGamma[t][j][i];
			}
		}
	}
	std::cout << "Y and DiGamma complete" << std::endl;
}
void Prediction::reEstimate()
{
	// re-estimate initial states
	for(std::size_t i = 0; i < init.col(); ++i)
	{
		init[0][i] = gamma[0][0][i];
	}
	
	// re-estimate A
	for(std::size_t i = 0; i < a.col(); ++i)
	{
		for(std::size_t j = 0; j < a.row(); ++j)
		{	
			numer = 0.0f;
			denom = 0.0f;
			for(std::size_t t = 0; t < duck.GetSeqLength() - 2; ++t)
			{
				numer = numer + diGamma[t][j][i];
				denom = denom + gamma[t][0][i];
			}
			a[j][i] = numer / denom;
		}
	}
	
	// re-estimate B
	for(std::size_t i = 0; i < a.row(); ++i)
	{
		for(std::size_t j = 0; j < b.col(); ++j)
		{
			numer = 0.0f;
			denom = 0.0f;
			for(std::size_t t = 0; t < duck.GetSeqLength() - 2; ++t)
			{
				if(getObservedState(t) == j)
					numer = numer + gamma[t][0][i];
				denom = denom + gamma[t][0][i];
			}
			b[i][j] = numer/denom;		
		}
	}
	std::cout << "re-estimate complete" << std::endl;
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
	for(std::size_t i = 0; i < combAction.size(); ++i)
	{
		if(combAction[i].isObservation(action.GetVAction(), action.GetHAction(), action.GetMovement()))
			return (std::size_t)combAction[i].obs;
	}
	return (std::size_t)(Other);
}


bool Prediction::CombinedAction::isObservation(ducks::EAction vAction, ducks::EAction hAction, ducks::EMovement movement)
{
	if(vA == vAction && hA == hAction && m == movement)
		return true;
	else
		return false;
}
