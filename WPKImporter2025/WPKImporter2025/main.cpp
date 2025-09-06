
#include <iostream>

#include "WpkImporter.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	WpkImporterOverlay my_wpk_importer(cin, cout, argc, argv);

	exit(EXIT_SUCCESS);
}


