// ModelMuxer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

typedef struct {
	int previousModel;
	double transitionDuration;
	double maxTransitionTimer;
	double output;
	bool isInitailzed;
	double epsilon;
} modelMuxer;

void initModelMuxer(modelMuxer* model, double maxTransitionTimer, double epsilon) {
	model->isInitailzed = true;
	model->maxTransitionTimer = maxTransitionTimer;
	model->previousModel = -1;
	model->epsilon = epsilon;
}

double muxModels(double* models, size_t numModels, size_t selectedModel, double dt, modelMuxer* state) {
	if (!state->isInitailzed) {
		std::cout << "Error: Please initialize first" << std::endl;
		return 0.0;
	}
	else if (dt <= 1e-6) {
		std::cout << "Error: Please pick a valid dt > 0" << std::endl;
		return 0.0;
	}
	else if (selectedModel >= numModels) {
		std::cout << "Error: Please pick a model within range" << std::endl;
		return 0.0;
	}
	else if (numModels <= 0) {
		std::cout << "Error: Please input at least one model" << std::endl;
		return 0.0;
	}

	if (state->previousModel < 0) {
		/* This is the first model the user's selected so there's no transition */
		state->previousModel = selectedModel;
		state->transitionDuration = 0.0;
		state->output = models[selectedModel];
	}
	else {
		if (state->previousModel != selectedModel) {
			/* Reset transition timer */
			state->transitionDuration = state->maxTransitionTimer;
		}
		if (state->transitionDuration > 0) {
			/* We're actively transitioning through there */
			
			if (fabs(state->output - models[selectedModel]) <= state->epsilon) {
				/* The two models have converged, end transition */
				state->output = models[selectedModel];
				state->transitionDuration = 0;
			}
			else {
				/* The models haven't converged, try making them closer */
				double modelDifference = (models[selectedModel] - state->output);
				/* We're already confident that dt and transitionDuration > 0 */
				double calculatedStep = modelDifference / (state->transitionDuration / dt);
				if (fabs(calculatedStep) > fabs(modelDifference)) {
					/* If transitionDuration < dt, calculatedStep will be > modelDifference and cause undesireable overshoot */
					state->output += modelDifference;
				}
				else {
					state->output += calculatedStep;
				}
				state->transitionDuration -= dt;
			}
		}
		else {
			/* We're not transitioning, so use model output */
			state->output = models[selectedModel];
		}
	}

	/* Update shadow of input*/
	state->previousModel = selectedModel;

	/* Return transitioned output */
	return state->output;
}

int main()
{
	modelMuxer muxer;
	static const double dt = 0.02;
	static const double max_duration = 1.0;
	static const int max_duration_counts = (int) (max_duration / dt);
	initModelMuxer(&muxer, max_duration, 0.01);
	double models[] = { 1.0, 3.0 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < (max_duration_counts / 2); j++) {
			std::cout << "Muxed: " << muxModels(models, sizeof(models) / sizeof(double), i % 2, dt, &muxer) << std::endl;
		}
	}
}