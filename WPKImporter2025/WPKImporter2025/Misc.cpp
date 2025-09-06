#include "WpkImporter.hpp"


string Misc::ToUppercase(const string& input)
{
	string result = input;
	transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return toupper(c); });
	return result;
}