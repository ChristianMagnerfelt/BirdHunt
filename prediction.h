#ifndef PREDICTION_H
#define PREDICTION_H

#include "matrix.h"
#include "cduck.h"

#include<vector>

class Prediction
{
	public:
		Prediction(ducks::CDuck & refDuck) : duck(refDuck){}

		void calculateAlpha();
		void calculateBeta();
		void calculateYAndDiGamma();
		std::size_t getObservedState(std::size_t t);

	private:
		ducks::CDuck & duck;
		int c;
		int denom;
		Matrix initStates;
		Matrix obsStates;
		std::vector<Matrix> alpha;
		std::vector<Matrix> beta;
};
#endif
