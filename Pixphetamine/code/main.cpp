/* Standard library includes */
#include <iostream>
#include <fstream>
#include <string>

/* Internal headers includes */
#include "SCoreEngine.h"
#include "SErrorHandler.h"

/* Entry point */
pxInt main(pxInt argc, char *argv[]) {
//#define DEBUG
	std::cout.precision(2);

// If in debug mode, print cerr in console, else in log.txt
#ifdef DEBUG

#else
	/*std::ofstream logStream("log.txt");
	std::ofstream errorStream("error.txt");
	std::streambuf* originErrorStream = std::cerr.rdbuf();
	std::cerr.rdbuf(errorStream.rdbuf());
	*/
#endif

	Utility::SErrorHandler* ErrorLog = &Utility::SErrorHandler::getInstance();
	ErrorLog->setOutputFile("error.txt");

	SCoreEngine* Engine = &SCoreEngine::getInstance();

	Engine->runGameLoop();

	SCoreEngine::destroyInstance();

	Utility::SErrorHandler::destroyInstance();

	return EXIT_SUCCESS;
}