#include "DebugController.h"


void DebugController::printDebug(std::string message)
{
	if(DebugActive)
	{
		std::cout<<message;
	}
}