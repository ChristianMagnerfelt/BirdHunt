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
		Matrix initM = {{0.33,0.34,0.33}};
		initM.print();
		std::cout << std::endl;
		Matrix statesM = {{0.36,0.31,0.33},{0.30,0.37,0.33},{0.31,0.31,0.38}};
		statesM.print();
		std::cout << std::endl;
		Matrix obsM = {	{0.14,0.09,0.07,0.15,0.10,0.12,0.14,0.08,0.11},
						{0.12,0.14,0.08,0.11,0.14,0.09,0.07,0.15,0.10},
						{0.15,0.10,0.12,0.14,0.08,0.11,0.14,0.09,0.07}};
		obsM.print();
		std::cout << std::endl;
		

		std::cout << "Predicting for duck " << 0 << std::endl;
		Prediction p(pState.GetDuck(0), initM, statesM, obsM);
		p.calculate();

		std::cout << "Next V : " << (ducks::EAction)p.getNextV() << std::endl;
		std::cout << "Next H : " << (ducks::EAction)p.getNextH() << std::endl;
		
		//this line doesn't shoot any bird
		return CAction(0,(ducks::EAction)p.getNextV(),(ducks::EAction)p.getNextH(),BIRD_STOPPED);

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
