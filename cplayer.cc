#include "cplayer.h"
#include <cstdlib>
#include <iostream>
#include "matrix.h"

namespace ducks
{
	CPlayer::CPlayer()
	{

	}

	CAction CPlayer::Shoot(const CState &pState,const CDeadline &pDue)
	{
		/*
		 * Here you should write your clever algorithms to get the best action.
		 * This skeleton never shoots.
		 */
		Matrix stateM = {{0.24,0.26,0.24,0.26},{0.24,0.26,0.24,0.26},{0.24,0.26,0.24,0.26}};
		stateM.print();
	
	
	
	
		//this line doesn't shoot any bird
		return cDontShoot;

		//this line would predict that bird 0 is totally stopped and shoot at it
		//return CAction(0,ACTION_STOP,ACTION_STOP,BIRD_STOPPED);
	}

	void CPlayer::Guess(std::vector<CDuck> &pDucks,const CDeadline &pDue)
	{
		/*
		 * Here you should write your clever algorithms to guess the species of each alive bird.
		 * This skeleton guesses that all of them are white... they were the most likely after all!
		 */
		 
		for(int i=0;i<pDucks.size();i++)
		{
		     if(pDucks[i].IsAlive())
		         pDucks[i].SetSpecies(SPECIES_WHITE);
		}
	}
	
	void CPlayer::Hit(int pDuck,ESpecies pSpecies)
	{
		std::cout << "HIT DUCK!!!" << std::endl;
	}
	
	Matrix CPlayer::forward(const Matrix & states, const Matrix & initState, const Matrix & obsMatrix, const CDuck & duck, int & cT)
	{
		if(initState.col() != obsMatrix.row())
			throw;
			
		// Compute alpha 0
		Matrix alpha(1, initState.col());
		int c0 = 0.0f;
		for(std::size_t i = 0; i < initState.col(); ++i)
		{
			alpha[0][i] = initState[0][i] * obsMatrix[i][getObservedState(duck, 0)];
			c0 += alpha[0][i];
		}
		
		// Scale alpha 0
		c0 = 1/c0;
		for(std::size_t i = 0; i < alpha.col(); ++i)
		{
			alpha[0][i] = c0 * alpha[0][i];
		}

		// Compute alpha t
		Matrix result;		
		Matrix prevT = alpha;
		for(std::size_t t = 1; t < duck.GetSeqLength(); ++t)
		{
			cT = 0;
			Matrix alphaT(1, alpha.col());
			
			for(std::size_t i = 0; i < states.row(); ++i)
			{
				for(std::size_t j = 0; j < states.col(); ++j)
				{
					alphaT[0][i] = alphaT[0][i] + prevT[0][j] * states[j][i];
				}
				alphaT[0][i] = alphaT[0][i] * obsMatrix[i][getObservedState(duck, t)];
				cT += alphaT[0][i];
			}
			
			cT = 1/cT;
			for(std::size_t i = 0; i < states.row(); ++i)
			{
				alphaT[0][i] = cT * alphaT[0][i];
			} 
			// Scale alpha t
			prevT = alphaT;
		}

		return prevT;
	}

	std::size_t CPlayer::getObservedState(const CDuck & duck, std::size_t t) const
	{
		return 0;	
	}
	
	Matrix CPlayer::backward(const Matrix & states, const Matrix & obs, const CDuck & duck, int & cT)
	{
			Matrix betaT(1, states.row());
			for(std::size_t i = 0; i < states.row(); ++i)
			{	
				betaT[0][i] = cT;
			}
			
			Matrix betaNext(1, states.row());
			int prevT = duck.GetSeqLength() - 1;
			for(int t = prevT - 1; t >= 0; --t)
			{
				for(std::size_t i = 0; i < states.row(); ++i)
				{
					betaNext[0][i] = 0;
					for(std::size_t j = 0; j < states.row(); ++j)
					{
						betaNext[0][i] = betaNext[0][i] + states[j][i] * obs[j][getObservedState(duck, prevT)] * betaT[0][j]; 
					}
					// Scale beta with cT
					betaNext[0][i] = cT * betaNext[0][i]; 					
				}

				betaT = betaNext;
				prevT = t;
			}
			return betaT;
	}
	void CPlayer::calculateYAndDiGamma(const Matrix & alpha, const Matrix & beta, const Matrix & states, const Matrix & obs, const CDuck & duck, Matrix & y, Matrix & diGamma, int & denom)
	{
		for(std::size_t t = 0; t < duck.GetSeqLength() - 1; ++t)
		{
			denom = 0;
			for(std::size_t i = 0; i < states.row(); ++i)
			{
				for(std::size_t j = 0; j < states.row(); ++j)
				{
					denom = denom + alpha[0][i] * states[j][i] * obs[j][getObservedState(duck, t)];
				}
			}
		}
	}	
/*namespace ducks*/ }
