
#include <iostream>
#include "WpkExporter.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    WpkExporter my_wpk_exporter(cin, cout, argc, argv);
    exit(EXIT_SUCCESS);
}

