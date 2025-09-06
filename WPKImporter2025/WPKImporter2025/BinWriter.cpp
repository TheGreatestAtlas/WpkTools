#include "WpkImporter.hpp"

void BinWriter::WriteString(vector<char>& output_file_buff, string& str)
{
    output_file_buff.insert(output_file_buff.end(), str.begin(), str.end());
    BW::WriteValue<char>(output_file_buff, '\0');
}

void BinWriter::SaveOutputFileBufferToBinFile(string& output_file_name, const vector <char>& output_file_buffer)
{
	fstream output_file;
	output_file.open(output_file_name, std::ios::out | std::ios::binary);

	output_file.write(reinterpret_cast<const char*>(&output_file_buffer[0]), output_file_buffer.size());

	output_file.close();
}