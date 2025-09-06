#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <unordered_map>

#ifdef _DEBUG
	#define DEBUG_PRINT(x) printf_s("%s",x)
#else
	#define DEBUG_PRINT(x)
#endif

using namespace std;

const string wavepack_namespace_name = "WAVEPACK";
const string wavepack_namespace_name_with_space = ::wavepack_namespace_name + " ";

const string level_struct_name = "LEVEL";
const string level_struct_name_with_space = ::level_struct_name + " ";

const string wave_struct_name = "WAVE";
const string wave_struct_name_with_space = ::wave_struct_name + " ";

const string type_instruction_name = "TYPE";
const string disable_level_2_instruction_name = "DISABLELEVEL2";

constexpr int64_t initialize_signal_value = -1;
constexpr int64_t push_back_signal_value = 0;

constexpr uint32_t old_wpk_format_type = 0;
constexpr uint32_t new_wpk_format_type = 1;

const string delayed_const_string = "delayed";
const string delayed_const_string_uppercase = "DELAYED";
constexpr uint32_t delayed_const_value = 1;

const string null_const_string_uppercase = "NULL";
constexpr uint32_t null_const_string_value = 0;

const string count_function_name = "count()";

const string measure_all_value_sizes_after_this_instruction_function_name 
			= "measure_all_value_sizes_after_this_instruction()";


const string wpk_format = ".wpk";
const string cpp_format = ".cpp";
const string txt_format = ".txt";

constexpr bool success_value = false;
constexpr bool failure_value = true;

namespace fs = filesystem;

namespace Misc
{
	string ToUppercase(const string& input);
}


namespace BinWriter
{
	template<typename T>
	void WriteValue(vector<char>& buff, const T& val)
	{
		const char* ptr = reinterpret_cast<const char*>(&val);
		buff.insert(buff.end(), ptr, ptr + sizeof(T));
	}

	template<typename T>
	void WriteTabFromTo(vector<char>& buff, const T tab[], const int from, const int to)
	{
		for (int i = from; i <= to; ++i)
			WriteValue(buff, tab[i]);
	}

	template<typename T>
	void WriteTabFromToReverse(vector<char>& buff, const T tab[], const int from, const int to)
	{
		for (int i = from; i >= to; --i)
			WriteValue(buff, tab[i]);
	}


	template<typename T>
	void WriteTabViaIndexes(vector<char>& buff, const T tab[], const std::vector<int> indexes)
	{
		for (int index : indexes)
		{
			WriteValue(buff, tab[index]);
		}
	}

	void WriteString(vector<char>& output_file_buff, string& str);

	void SaveOutputFileBufferToBinFile(string& output_file_name, const vector <char>& output_file_buffer);
}

namespace BW = BinWriter;



//---------------------
// InputBinFile CLASS
//---------------------

class InputBinFile
{
public:

	uint8_t* m_file_buffer;

	InputBinFile(string argfilename, string argmode = "text");
	~InputBinFile();

	void Informations(ostream& o) const;
	void ExitWhenFileDontExist(ostream& o) const;


	bool GetIsOpened() const;
	bool GetIsOpenNow() const;
	uint64_t GetFileLength() const;

private:


	fstream m_input_file;
	string m_file_name;
	uint64_t m_file_length;
	bool m_is_opened;
	bool m_is_open_now;

	string m_work_mode;


	void OpenAndReadFile();

};

//------------
// Compilator
//------------

class CppCompilator
{
public:

	CppCompilator(uint8_t* argfilebuffer = nullptr, uint64_t argfilelength = 0);

	void CompileCppFileToBin();

	const vector<char>& GetOutputFileBuffer() const;

private:
	vector<char> output_file_buffer = {};

	bool m_measure_error_signal;
	size_t m_data_size_offset;

	constexpr const unsigned int static c_number_of_levels = 3;
	bool m_arr_of_signals_for_levels[c_number_of_levels] = { 0 };
	size_t m_arr_of_offsets_for_number_of_waves[c_number_of_levels] = { 0 };
	uint32_t m_arr_of_number_of_waves[c_number_of_levels] = { 0 };

	int32_t m_actual_level_index;

	bool m_its_string;
	bool m_its_comment;
	uint8_t* m_file_buffer;
	uint64_t m_file_length;

	string m_instruction;
	string m_value_type;
	string m_name;
	string m_value;


	vector <uint8_t> m_clean_file = {};

	unordered_map<string, uint32_t> m_list_of_consts = {};

	//Metody:
	template<typename T>
	void WriteRaw(vector<char>& buf, const T& val)
	{
		const char* ptr = reinterpret_cast<const char*>(&val);
		buf.insert(buf.end(), ptr, ptr + sizeof(T));
	}

	void DeleteComments();
	void DeleteContrCharAndPutToVector();

	void WriteGUID(string& slowo);
	void WriteString(string& str);

	template <typename T>
	void WriteValueToFile(string& value, uint8_t mode);
	
	template <typename T>
	void WriteTabToFile(string& str, uint8_t mode);


	void GetValuesFromString();
	void WriteInstructionToBin(string& valuetype, string& name, string& value);

	uint64_t CountValuesInTab(string& value);

	void InitializeHashMap();

};


class Wave
{
public:
	Wave(uint32_t arg_wavepack_type = ::new_wpk_format_type);
	~Wave();

	void GetFromFile(string& uppercased_instruction, string& original_instruction, size_t& line_number);
	void WriteToFile(vector<char>& output_file_buff);

	bool GetError() const;
	uint32_t GetSizeOf();

	size_t GetLineNumber() const;

private:
	bool m_error;

	size_t m_line_number;

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

	void GetFromFile(string& uppercased_instruction, string& original_instruction, size_t& line_number);
	void DebugTheObject(ostream& arg_o);
	void WriteToFile(vector<char>& output_file_buff);

	string GetWavePackName() const;
private:
	string m_wavepack_name;

	uint32_t m_wavepack_type;
	uint32_t m_special_hh_and_ww3_case;

	constexpr const unsigned int static c_number_of_levels = 3;

	uint32_t m_number_of_waves_in_level[c_number_of_levels] = { 0 };

	vector<Wave> m_wave_vector_level[c_number_of_levels] = {};

	uint32_t m_level_index;
	uint32_t m_number_of_waves_in_level_checksum[c_number_of_levels] = {0};

	uint32_t m_data_size;

	bool m_level_mode;

	bool m_waves_error[c_number_of_levels] = { false };

	size_t m_type_instruction_line_number;
	size_t m_disable_level_2_instruction_line_number;
	size_t m_level_instruction_line_number[c_number_of_levels] = { 0 };

};




//Txt Utility:
namespace TxtUtility
{
	void MoveByLines(stringstream& arg_stream, int count);

	template<typename T>
	bool GetOneValue(string& arg_line, string instruction_name, const char* format, T& dst)
	{
		instruction_name += " ";

		const size_t intruction_name_len = instruction_name.length();

		if (arg_line.substr(0, intruction_name_len) == instruction_name)
		{
			string m_help_str = arg_line.substr(intruction_name_len, arg_line.length() - intruction_name_len);
			sscanf_s(m_help_str.c_str(), format, &dst);

			DEBUG_PRINT("VALUE = ");
			DEBUG_PRINT(to_string(dst).c_str());
			DEBUG_PRINT("\n");

			return ::success_value;
		}

		return ::failure_value;
	}

	bool GetSingleString(string& arg_line, string& instruction_name, string& dst);

	bool GetObjType(
		string& uppercased_instruction, 
		string& original_instruction, 
		string& inside_obj_type, 
		string& inside_obj_name, 
		string obj_type_name
	);


	bool GetLevelHeader(string& arg_line,
		uint32_t& arg_level_index,
		uint32_t arg_number_of_waves_in_level_checksum[],
		bool& arg_level_mode);

	void GetWaves(string& uppercased_instruction,
		string& original_instruction,
		uint32_t& arg_wavepack_type,
		vector<Wave>& arg_wave_vector_level,
		uint32_t& arg_number_of_waves_in_level_checksum,
		bool& arg_level_mode,
		size_t& line_number);

	bool CheckWaves(string& uppercased_instruction,
		uint32_t& arg_number_of_waves_in_level_checksum,
		bool& arg_level_mode);

	inline void PrepareString(string& arg_str, string sub_str)
	{
		if (arg_str.find(sub_str) != string::npos)
		{
			arg_str = sub_str;
		}
	}

}

namespace TU = TxtUtility;



//----------------------------------
// Whole Wavepack Container
//----------------------------------

class WholeWavepacksContainer
{
public:
	vector<WavePack> m_wavepacks = {};
	WavePack m_single_wavepack;
};

//-------------------
// Class TxtCompiler
//-------------------

class TxtCompilator
{
public:
	// metody:
	TxtCompilator(uint8_t* argfilebuffer = nullptr, uint64_t argfilelength = 0, ostream& arg_o = cout);

	void CompileTxtFileToBinBuffers();
	void SaveOutputFileBuffersToBinFiles(fs::path& arg_input_path);
private:

	size_t m_line_number;

	WholeWavepacksContainer m_whole_particle_emiter_container = {};

	//Pola:
	bool m_its_comment;

	int64_t bracket_counter;

	string m_obj_name;
	string m_inside_obj_type;
	string m_inside_obj_name;

	uint8_t* m_file_buffer;
	uint64_t m_file_length;

	vector <char> m_clean_file = {};

	string m_instruction;

	ostream& m_o;

	//Metody:
	void DeleteComments();
	void CleanFileBuffer(vector<char>& arg_clean_file, bool change_newline_switch = true);

	void InterpretTxtWavePacksInstructions(
		string& uppercased_instruction, 
		string& original_instruction
	);

	template<class T>
	void CompileObject(
		string& uppercased_instruction, 
		string& original_instruction, 
		T& single_object, 
		vector<T>& objects_vec
	)
	{

		if (uppercased_instruction == "{" && bracket_counter == initialize_signal_value)
		{
			++bracket_counter;
			DEBUG_PRINT("Initialize...\n");
			single_object = T(m_inside_obj_name);
		}

		if (uppercased_instruction == "{")
		{
			++bracket_counter;
		}

		if (bracket_counter > 0)
		{
			single_object.GetFromFile(uppercased_instruction, original_instruction, this->m_line_number);
		}

		if (uppercased_instruction == "}")
		{

			--bracket_counter;
		}

		if (uppercased_instruction == "}" && bracket_counter == push_back_signal_value)
		{
			DEBUG_PRINT("DEBUGGING OBJECT!\n");

			single_object.DebugTheObject(m_o);

			DEBUG_PRINT("PushBack\n");
			objects_vec.push_back(single_object);
			single_object = T();
			bracket_counter = initialize_signal_value;

			m_inside_obj_type = string();
			m_inside_obj_name = string();
		}

	}

};




class WpkImporterOverlay
{
public:
	//Metody:
	WpkImporterOverlay(istream& argin, ostream& argo, int my_argc, char* my_argv[]);

private:
	//Pola:
	constexpr const unsigned int static c_correct_number_of_args = 2;

	istream& m_in_stream;
	ostream& m_o;

	string m_file_name;

	//metody:

	void StandardProgramExecution();

};

