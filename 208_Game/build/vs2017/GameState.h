#pragma once

//all the states used int the game 
enum class State { Init,MENU, OPTIONS, CONTROLS, PAUSE_CONTROLS, LEVEL, PAUSE, CREDITS, DEATH };

class GameState
{
public:
	// Set the current state
	void setCurrentState(State s);
	// Returns the current state.
	State getCurrentState();

protected:
	State currentState;
};

