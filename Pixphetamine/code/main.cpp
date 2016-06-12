/* Standard library includes */
#include <iostream>
#include <fstream>
#include <string>

/* Internal headers includes */
#include "UCoreEngine.h"
#include "UErrorHandler.h"

/* Entry point */
pxInt main(pxInt argc, char *argv[]) {
//#define DEBUG
	std::cout.precision(2);

// If in debug mode, print cerr in console, else in log.txt
#ifdef DEBUG

#else
	std::ofstream logStream("log.txt");
	std::streambuf* originErrorStream = std::cerr.rdbuf();
	std::cerr.rdbuf(logStream.rdbuf());
	
#endif

	Utility::UErrorHandler* ErrorLog = &Utility::UErrorHandler::getInstance();
	ErrorLog->setOutputFile("error.txt");

	UCoreEngine* Engine = &UCoreEngine::getInstance();

	Engine->runGameLoop();

	UCoreEngine::destroyInstance();

	Utility::UErrorHandler::destroyInstance();

	return EXIT_SUCCESS;
}