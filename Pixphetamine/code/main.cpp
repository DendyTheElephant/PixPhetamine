/* Standard library includes */
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

/* Internal headers includes */
#include "SCoreEngine.h"


/* Entry point */
pxInt main(pxInt argc, char *argv[]) {

	std::cout.precision(2);

// If in debug mode, print cerr in console, else in log.txt
#ifdef DEBUG

#else
	std::ofstream out("log.txt");
	std::streambuf* orig = std::cerr.rdbuf();
	std::cerr.rdbuf(out.rdbuf());
#endif

	try {
		
		SCoreEngine* Engine = &SCoreEngine::getInstance();

		Engine->runGameLoop();

		SCoreEngine::destroyInstance();
	
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << "\n";
	}
	

	return EXIT_SUCCESS;
}