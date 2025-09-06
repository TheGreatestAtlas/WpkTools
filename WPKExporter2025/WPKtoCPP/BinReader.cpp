#include "WpkExporter.hpp"

string BinReader::GetString(uint8_t* buff, size_t& curr_offset)
{
    string simple_string = string();

    uint8_t single_char = 'a';

    while (single_char != '\0')
    {
        single_char = BR::GetBinVal<uint8_t>(buff, curr_offset);

        if (single_char != '\0')
        {
            simple_string += single_char;
        }
    }

    return simple_string;
}
