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
	
	Matrix CPlayer::forward(const Matrix & states, const Matrix & initState, const Matrix & obsMatrix, std::size_t obsCol, const CDuck & duck)
	{
		if(initState.col() != obsMatrix.row())
			throw;
			
		// Compute alpha 0
		Matrix alpha(1, initState.col());
		float c0 = 0.0f;
		for(std::size_t i = 0; i < initState.col(); ++i)
		{
			alpha[0][i] = initState[0][i] * obsMatrix[i][obsCol];
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
		for(std::size_t t = 0; t < duck.GetSeqLength(); ++t)
		{
			int cT = 0;
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
	
	Matrix CPlayer::backward()
	{

	}
	
/*namespace ducks*/ }
