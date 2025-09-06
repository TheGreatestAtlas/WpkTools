#include "WpkImporter.hpp"


InputBinFile::InputBinFile(string argfilename, string argmode) :
	m_file_name(argfilename),
	m_work_mode(argmode),
	m_is_opened(false),

	m_is_open_now(false),
	m_file_length(0),
	m_file_buffer(nullptr)
{
	DEBUG_PRINT("Nazwa pliku:");
	DEBUG_PRINT(this->m_file_name.c_str());
	DEBUG_PRINT("\n");

	OpenAndReadFile();

}

void InputBinFile::OpenAndReadFile()
{

	if (this->m_work_mode == "binary")
	{
		m_input_file.open(m_file_name, ios::in | ios::binary | ios::ate);
	}

	if (this->m_work_mode == "text")
	{
		m_input_file.open(m_file_name, ios::in | ios::ate);
	}

	if ((this->m_work_mode != "text") && (this->m_work_mode != "binary"))
	{
		DEBUG_PRINT("You have entered an invalid mode in the constructor of the InputBinFile class.\n");
		DEBUG_PRINT("You wrote: ");
		DEBUG_PRINT(m_work_mode.c_str());
		DEBUG_PRINT("\nThis will be interpreted as text mode.\n");

		m_input_file.open(m_file_name, std::ios::in | ios::ate);
	}

	if (m_input_file)
	{
		this->m_is_opened = this->m_is_open_now = true;
		this->m_file_length = this->m_input_file.tellg();
		this->m_input_file.seekg(0);
		this->m_file_buffer = new uint8_t[this->m_file_length];
		this->m_input_file.read(reinterpret_cast<char*>(this->m_file_buffer), this->m_file_length);
		this->m_input_file.close();
		this->m_is_open_now = false;
	}

}

bool InputBinFile::GetIsOpened() const { return this->m_is_opened; }
bool InputBinFile::GetIsOpenNow() const { return this->m_is_open_now; }
uint64_t InputBinFile::GetFileLength() const { return this->m_file_length; }

InputBinFile::~InputBinFile()
{
	if (this->m_is_open_now == false && this->m_is_opened == true)
		delete[] this->m_file_buffer;

}

void InputBinFile::Informations(ostream& o) const
{
	if (this->m_is_opened == 0)
	{
		o	<< endl
			<< "ERROR: The specified file could not be opened." << endl
			<< "-------------------------------------" << endl
			<< "Opening the file:" << this->m_file_name << endl
			<< "Was the file successfully opened?: " << this->m_is_opened << endl
			<< "Is the file currently open?: " << this->m_is_open_now << endl
			<< "-------------------------------------" << endl
			<< endl;
	}

}

void InputBinFile::ExitWhenFileDontExist(ostream& o) const
{
	if (this->m_is_opened == false && this->m_is_open_now == false)
	{
		o << "The specified file does not exist!" << endl;
		exit(EXIT_FAILURE);
	}
}
