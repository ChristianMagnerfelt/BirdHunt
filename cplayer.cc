#include "cplayer.h"
#include <cstdlib>
#include <iostream>
#include "matrix.h"
#include "prediction.h"

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
		Matrix initM = {{0.34,0.33,0.33}};
		initM.print();
		std::cout << std::endl;
		Matrix statesM = {{0.34,0.33,0.33},{0.34,0.33,0.33},{0.34,0.33,0.33}};
		statesM.print();
		std::cout << std::endl;
		Matrix obsM = {	{0.12,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11},
						{0.12,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11},
						{0.12,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11}};
		obsM.print();
		std::cout << std::endl;
			
		for(int i = 0; i < pState.GetNumDucks(); ++i)
		{
			std::cout << "Predicting for duck " << i << std::endl;
			Prediction p(pState.GetDuck(i), initM, statesM, obsM);
			p.calculate();
		}
	
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
/*namespace ducks*/ }
