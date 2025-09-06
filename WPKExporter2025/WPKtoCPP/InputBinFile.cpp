#include "WpkExporter.hpp"

//------------------------
// Input File Buffer Class
//------------------------

InputBinFile::InputBinFile(char* argfilename) :

	m_file_name(argfilename),
	m_is_opened(false),
	m_is_open_now(false),
	m_file_length(0),
	m_file_buffer(nullptr)
{
	DEBUG_PRINT("Nazwa pliku:");
	DEBUG_PRINT(m_file_name);
	DEBUG_PRINT("\n");

	OpenFile();

	if (m_is_open_now == true)
	{
		MeasureFile();
		PutFileToBuffer();
		m_input_file.close();
		m_is_open_now = m_input_file.is_open();

	}
}

void InputBinFile::OpenFile()
{
	m_input_file.open(m_file_name, ios::in | ios::binary);

	m_is_opened = m_input_file.is_open();
	m_is_open_now = m_is_opened;

}

void InputBinFile::MeasureFile()
{
	m_input_file.seekg(0, ios_base::end);
	m_file_length = m_input_file.tellg();
	m_input_file.seekg(0, ios_base::beg);
}


void InputBinFile::PutFileToBuffer()
{
	m_file_buffer = new uint8_t[m_file_length];
	m_input_file.read(reinterpret_cast<char*>(m_file_buffer), m_file_length);

}

bool InputBinFile::GetIsOpened() const { return m_is_opened; }
bool InputBinFile::GetIsOpenNow() const { return m_is_open_now; }
size_t InputBinFile::GetFileLength() const { return m_file_length; }

InputBinFile::~InputBinFile()
{
	if (m_is_open_now == false && m_is_opened == true)
		delete[] m_file_buffer;

}

void InputBinFile::Informations(ostream& o) const
{
	o	<< endl
		<< "-------------------------------------"					 << endl
		<< "Opening the file:"						<< m_file_name	 << endl
		<< "Was the file successfully opened?: "	<< m_is_opened	 << endl
		<< "Is the file currently open?: "			<< m_is_open_now << endl
		<< "-------------------------------------"					 << endl
		<< endl;
}


void InputBinFile::ExitWhenFileDontExist(ostream& o) const
{
	if (m_is_opened == false && m_is_open_now == false)
	{
		o << "The specified file does not exist!" << endl;
		exit(EXIT_FAILURE);
	}
}