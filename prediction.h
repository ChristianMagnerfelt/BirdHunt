#ifndef PREDICTION_H
#define PREDICTION_H

#include "matrix.h"
#include "cduck.h"

#include<vector>

class Prediction
{
	public:
		enum Observation
		{
			StableEast,
			StableWest,
			SlowUpEast,
			SlowUpWest,
			SlowDownEast,
			SlowDownWest,
			FastUpEast,
			FastUpWest,
			FastDownEast,
			FastDownWest,
			FallEast,
			FallWest,
			Stopped,
			Other,
		};
			
		struct CombinedAction{
			CombinedAction(ducks::EAction vAction, ducks::EAction hAction, ducks::EMovement movement, Observation observation) :
				vA(vAction), hA(hAction), m(movement), obs(observation){}
			bool isObservation(ducks::EAction vAction, ducks::EAction hAction, ducks::EMovement movement);
			ducks::EAction vA;
			ducks::EAction hA;
			ducks::EMovement m;
			Observation obs;
		};
			
		Prediction(const ducks::CDuck & refDuck, const Matrix & initStates, const Matrix & states, const Matrix & obsStates);
		void calculate();
		void print() const;
	private:
		std::size_t getObservedState(std::size_t t);
		void calculateAlpha();
		void calculateBeta();
		void calculateYAndDiGamma();
		void calculateLogProb();
		void reEstimate();
		
		// Bird variables
		const ducks::CDuck & duck;
		static std::vector<CombinedAction> combAction;
			
		// HMM variables
		std::vector<float> c;
		float denom;
		float numer;
		float logProb;
		float oldLogProb;
		std::size_t maxIter;
		Matrix init;
		Matrix a;
		Matrix b;
		std::vector<Matrix> alpha;
		std::vector<Matrix> beta;
		std::vector<Matrix> diGamma;
		std::vector<Matrix> gamma;
};
#endif
