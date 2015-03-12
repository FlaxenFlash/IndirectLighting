#pragma once
#include <string>
#include <iostream>
class DebugController
{
private:
	static const bool DebugActive = true;
public:
	static void printDebug(std::string message);
};

