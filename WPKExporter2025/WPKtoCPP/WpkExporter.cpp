#include "WpkExporter.hpp"


void WpkExporter::ShowStartScreen()
{
     m_o << "***********************" << endl 
         << "|WPK exporter by ATLAS|" << endl
         << "**********2025*********" << endl;
}

void WpkExporter::StandardProgramExecution()
{

    const vector<string> correct_formats = { ::txt_format, ::cpp_format };

    ShowStartScreen();

    m_o << "Enter the name of the input file:" << endl;
    m_in_stream >> this->m_input_file_name;

    m_o << "Choose format [" << ::txt_format <<  "/" << ::cpp_format <<  "]:" << endl;
    
    do
    {
        m_in_stream >> this->m_output_format;

        if (!Misc::CheckAnswerWithEveryElementInVec(this->m_output_format, correct_formats))
        {
            m_o << "Choose from the given formats..." << endl;
        }

    } while (!Misc::CheckAnswerWithEveryElementInVec(this->m_output_format, correct_formats));

}

void WpkExporter::ArgcAndArgvExecution(string program_name)
{
    const vector<string> correct_formats = { ::txt_format, "-" + ::txt_format, ::cpp_format, "-" + ::cpp_format };

    if (!Misc::CheckAnswerWithEveryElementInVec(this->m_output_format, correct_formats))
    {
        m_o << "You have entered an incorrect output format." << endl
            << "Use: " 
            << program_name 
            << " <wpk_file> <" 
            << correct_formats[0]
            << "," 
            << correct_formats[1]
            << "," 
            << correct_formats[2]
            << "," 
            << correct_formats[3]
            << ">" 
            << endl;

        exit(EXIT_FAILURE);
    }

    if (this->m_output_format == correct_formats[1])
    {
        this->m_output_format = ::txt_format;
    }

    if (this->m_output_format == correct_formats[3])
    {
        this->m_output_format = ::cpp_format;
    }
}


WpkExporter::WpkExporter(istream& argin, ostream& argo, int my_argc, char* my_argv[]) :
    m_in_stream(argin), 
    m_o(argo), 
    m_input_file_name(string()),
    m_output_format(string())
{
    if (my_argc == this->c_correct_number_of_args)
    {
        this->m_input_file_name = *(my_argv + 1);
        this->m_output_format = *(my_argv + 2);
        ArgcAndArgvExecution(*(my_argv + 0));
    }
    else
    {
        StandardProgramExecution();
    }

    fs::path input_path = this->m_input_file_name;

    if (input_path.extension().string() != ("." + ::wpk_format))
    {
        m_o << "The specified file is not a .wpk file. The program terminates with an error." << endl;
        exit(EXIT_FAILURE);
    }

    //string wavepack_name = m_input_file_name.substr(0, m_input_file_name.length() - 4);

    string wavepack_name = input_path.stem().string();
    DEBUG_PRINT("wavepack_name: "); DEBUG_PRINT(wavepack_name.c_str()); DEBUG_PRINT("\n");

    InputBinFile input_wpk_file(const_cast<char*>(this->m_input_file_name.c_str()));
    input_wpk_file.Informations(m_o);
    input_wpk_file.ExitWhenFileDontExist(m_o);

    m_o << "Filename: " << this->m_input_file_name << endl;

    //string output_file_name = wavepack_name + "." + this->m_format;
    string output_file_name = (input_path.parent_path() / (wavepack_name + "." + this->m_output_format)).string();

    DEBUG_PRINT("ParentPath: "); DEBUG_PRINT(input_path.parent_path().string().c_str()); DEBUG_PRINT("\n");
    DEBUG_PRINT("output_file_name: "); DEBUG_PRINT(output_file_name.c_str()); DEBUG_PRINT("\n");

    size_t offset_in_file = 0;

    WavePack my_wavepack(wavepack_name);
    my_wavepack.GetFromFile(input_wpk_file.m_file_buffer, offset_in_file, input_wpk_file.GetFileLength());

    my_wavepack.ShowInformations(m_o);

    stringstream output_file_stream;

    if (this->m_output_format == ::txt_format)
    {
        output_file_stream << my_wavepack.ToOldFormatString();
    }

    if (this->m_output_format == ::cpp_format)
    {
        output_file_stream << my_wavepack.ToNewFormatString();
    }

    CW::WriteStreamToFile(output_file_stream, output_file_name);

    m_o << "DONE!" << endl;
}

WpkExporter::~WpkExporter()
{  
}