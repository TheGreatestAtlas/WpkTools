#include "WpkImporter.hpp"

WpkImporterOverlay::WpkImporterOverlay(istream& argin, ostream& argo, int my_argc, char* my_argv[]) : 
	m_in_stream(argin), 
	m_o(argo), 
	m_file_name(string())
{

	if (my_argc == this->c_correct_number_of_args)
	{
		this->m_file_name = *(my_argv + 1);
		m_o << "FileName = " << this->m_file_name << endl;
	}
	else
	{
		StandardProgramExecution();
	}

	fs::path input_path = this->m_file_name;

	string input_file_extension = input_path.extension().string();

	if (input_file_extension != ::cpp_format && input_file_extension != ::txt_format)
	{
		m_o << "The specified input file is not a .cpp file or a .txt file. The program terminates with an error." 
			<< endl;
		exit(EXIT_FAILURE);
	}

	string wavepack_name = input_path.stem().string();
	DEBUG_PRINT("wavepack_name: "); DEBUG_PRINT(wavepack_name.c_str()); DEBUG_PRINT("\n");

	//this->m_output_bin_file_name = this->m_file_name.substr(0, this->m_file_name.length() - 4) + ".wpk";
	string output_bin_file_name = (input_path.parent_path() / (wavepack_name + ::wpk_format)).string();

	DEBUG_PRINT("ParentPath: "); DEBUG_PRINT(input_path.parent_path().string().c_str()); DEBUG_PRINT("\n");
	DEBUG_PRINT("output_file_name: "); DEBUG_PRINT(output_bin_file_name.c_str()); DEBUG_PRINT("\n");

	if (input_file_extension == ::cpp_format)
	{
		m_o << "Output wavepack file:" << output_bin_file_name << endl;

		InputBinFile single_cpp_file(this->m_file_name, "binary");
		single_cpp_file.Informations(m_o);
		single_cpp_file.ExitWhenFileDontExist(m_o);

		CppCompilator my_cpp_compilator(single_cpp_file.m_file_buffer, single_cpp_file.GetFileLength());
		my_cpp_compilator.CompileCppFileToBin();

		BW::SaveOutputFileBufferToBinFile(output_bin_file_name, my_cpp_compilator.GetOutputFileBuffer());
	}

	if (input_file_extension == ::txt_format)
	{
		InputBinFile single_txt_file(this->m_file_name, "text");
		single_txt_file.Informations(m_o);
		single_txt_file.ExitWhenFileDontExist(m_o);

		TxtCompilator my_txt_compilator(single_txt_file.m_file_buffer, single_txt_file.GetFileLength());
		my_txt_compilator.CompileTxtFileToBinBuffers();
		my_txt_compilator.SaveOutputFileBuffersToBinFiles(input_path);
	}

	m_o << endl << "The " << this->m_file_name << " was compiled successfully..." << endl;

}


void WpkImporterOverlay::StandardProgramExecution()
{
	m_o << "*********************" << endl
		<< "|WPK Importer BY ATL|" << endl
		<< "*********2025********" << endl;

	m_o << "Enter the name of the input file:"; m_in_stream >> this->m_file_name;
	
}

