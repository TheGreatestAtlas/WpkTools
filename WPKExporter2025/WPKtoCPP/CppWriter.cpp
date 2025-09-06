#include "WpkExporter.hpp"

void CppWriter::WriteStreamToFile(stringstream& output_stream, string& output_file_name)
{
	fstream output_file;

	output_file.open(output_file_name, ios::out);
	auto output_stream_data(output_stream.str());
	output_file.write(&output_stream_data[0], output_stream_data.size());
	output_file.close();
}