#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

using namespace std;

#ifdef _DEBUG
	#define DEBUG_PRINT(x) printf_s("%s",x)
#else
	#define DEBUG_PRINT(x)
#endif

constexpr int number_of_bits_in_byte = 8;
constexpr uint32_t old_wpk_format_type = 0;
constexpr uint32_t new_wpk_format_type = 1;

const string txt_format = "txt";
const string cpp_format = "cpp";
const string wpk_format = "wpk";

const string bit_32_unique_key_type = "32bituniquekey";
const string struct_type = "struct";
const string string_type = "string";
const string namespace_type = "namespace";

const string wave_path_string_name = "wave_path";
const string wavepack_namespace_name = "wavepack";
const string wave_struct_name = "Wave";
const string level_struct_name = "Level";
const string data_size_value_name = "data_size";
const string type_value_name = "Type";
const string disable_level_2_value_name = "DisableLevel2";
const string number_of_waves_in_level_value_name = "number_of_waves_in_level_";
const string format_value_name = "format_";
const string wavepack_type_value_name = "wavepack_type";

const string delayed_const_string = "delayed";
const string delayed_const_string_uppercase = "DELAYED";
constexpr uint32_t delayed_const_value = 1;

const string null_const_string_uppercase = "NULL";
constexpr uint32_t null_const_string_value = 0;

const string count_function_name = "count()";
const string measure_all_value_sizes_after_this_instruction_function_name = "measure_all_value_sizes_after_this_instruction()";

namespace fs = filesystem;

enum StringFormatMode
{
	OldStringFormat,
	NewStringFormat
};

namespace Misc
{
	inline bool CheckAnswerWithEveryElementInVec(string& answer, const vector<string>& elements)
	{
		for (const auto& element : elements)
		{
			if(answer == element) 
				return true;
		}

		return false;
	}
}

//---------------------
// InputBinFile Class
//---------------------

class InputBinFile
{
public:

	uint8_t* m_file_buffer;

	void Informations(ostream& o) const;
	void ExitWhenFileDontExist(ostream& o) const;

	InputBinFile(char* argfilename);
	~InputBinFile();


	bool GetIsOpened() const;
	bool GetIsOpenNow() const;
	size_t GetFileLength() const;

private:

	fstream m_input_file;
	char* m_file_name;
	size_t m_file_length;
	bool m_is_opened;
	bool m_is_open_now;


	void OpenFile();
	void MeasureFile();
	void PutFileToBuffer();

};

namespace BinReader
{
	template<typename T>
	T GetBinVal(uint8_t* buff, size_t& curr_offset)
	{
		T value = reinterpret_cast<T&>(buff[curr_offset]);

		curr_offset += sizeof(T);

		return value;
	}

	string GetString(uint8_t* buff, size_t& curr_offset);

	template<typename T>
	void GetBinTab(T arr[], int arr_size, uint8_t* buff, size_t& curr_offset)
	{
		for (int i = 0; i < arr_size; ++i)
			arr[i] = GetBinVal<T>(buff, curr_offset);
	}
};

namespace BR = BinReader;

namespace CppWriter
{
	void WriteStreamToFile(stringstream& output_stream, string& output_file_name);

	template<typename Type>
	inline string ConvertTrivialTypeToString()
	{
		stringstream result = { };

		if (is_unsigned_v<Type>)
		{
			result << 'u';
		}

		result << "int" << sizeof(Type) * ::number_of_bits_in_byte << "_t";

		return result.str();
	}


	template<typename Type>
	inline string ConvertTrivialTypeToString(Type)
	{
		return ConvertTrivialTypeToString<Type>();
	}

	template<typename Type>
	inline void WriteTrivialValue(stringstream& ss, string name, Type& value)
	{
		ss << ConvertTrivialTypeToString(value) << " " << name << " = "; 
		
		if (sizeof(Type) * ::number_of_bits_in_byte == ::number_of_bits_in_byte)
		{
			ss << static_cast<uint32_t>(value) << ";";
		}
		else
		{
			ss << value << ";";
		}
	}


};

namespace CW = CppWriter;

class Wave
{
public:
	Wave(uint32_t arg_wavepack_type);
	~Wave();

	void GetFromFile(uint8_t* buff, size_t& curr_offset);
	string ToOldFormatString() const;
	string ToNewFormatString() const;

private:
	uint32_t m_wavepack_type;

	constexpr const unsigned int static a_size = 3;
	constexpr const unsigned int static b_size = 2;
	constexpr const unsigned int static c_size = 3;

	uint32_t m_a[a_size] = { 0 };
	uint8_t m_b[b_size] = { 0 };
	uint32_t m_c[c_size] = { 0 };
	string m_wave_path;
};

class WavePack
{
public:
	WavePack(string arg_wavepack_name = string());
	~WavePack();

	void GetFromFile(uint8_t* buff, size_t& curr_offset, size_t size_of_file);
	string ToOldFormatString();
	string ToNewFormatString();

	void ShowInformations(ostream& arg_o) const;

private:
	string m_wavepack_name;

	constexpr const unsigned int static format_size = 4;
	uint8_t m_format[format_size] = { 0 };

	uint32_t m_wavepack_type;

	bool m_special_hh_and_ww3_case; // hh = heli heroes game
									// ww3 = world war 3 black gold game

	constexpr const unsigned int static c_number_of_levels = 3;

	uint32_t m_number_of_waves_in_level[c_number_of_levels];
	uint32_t m_data_size;

	vector<Wave> m_wave_vector_level[c_number_of_levels] = {};

	void GetWaveVector(vector<Wave>& arg_wave_vector,
					   uint32_t& arg_number_of_waves,
				       uint8_t* buff,
					   size_t& curr_offset);

	void WriteWaveVector(vector<Wave>& arg_wave_vector,
		uint32_t& arg_number_of_waves,
		uint32_t& arg_level_number,
		stringstream& ss,
		StringFormatMode arg_string_format_mode);

};



class WpkExporter
{

public:

	WpkExporter(istream& argin, ostream& argo, int my_argc, char* my_argv[]);
	~WpkExporter();

private:
	constexpr const unsigned int static c_correct_number_of_args = 3;

    ostream& m_o;
    istream& m_in_stream;

    string m_input_file_name;

	string m_output_format;

	void ShowStartScreen();
	void StandardProgramExecution();
	void ArgcAndArgvExecution(string program_name);

};