/* Standard library includes */
#include <iostream>
#include <fstream>
#include <string>

/* Internal headers includes */
#include "SCoreEngine.h"


/* Entry point */
int main(int argc, char *argv[]) {

	// Redirect cerr to error.txt
	std::ofstream out("error.txt");
	std::streambuf* orig = std::cerr.rdbuf();
	std::cerr.rdbuf(out.rdbuf());

	std::cout.precision(2);


	SCoreEngine* Engine = &SCoreEngine::getInstance();
	
	Engine->runGameLoop();

	SCoreEngine::destroyInstance();

	return 0;
}