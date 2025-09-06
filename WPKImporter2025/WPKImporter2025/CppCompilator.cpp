#include "WpkImporter.hpp"

const vector<string> value_types = {
	"char",
	"char[]",

	"uint8_t",
	"uint8_t[]",

	"uint16_t",
	"uint16_t[]",

	"uint32_t",
	"uint32_t[]",

	"uint64_t",
	"uint64_t[]",

	"GUID",
	"string",
	"struct",

	"int8_t",
	"int8_t[]",

	"int16_t",
	"int16_t[]",

	"int32_t",
	"int32_t[]",

	"int64_t",
	"int64_t[]",

	"short",
	"short[]",

	"int",
	"int[]",

	"longint",
	"longint[]",

	"bool",
	"bool[]",

	"32bituniquekey",
	"namespace"
};


void CppCompilator::InitializeHashMap()
{
	m_list_of_consts[::null_const_string_uppercase] = ::null_const_string_value;
	m_list_of_consts[::delayed_const_string] = ::delayed_const_value;
	m_list_of_consts[::delayed_const_string_uppercase] = ::delayed_const_value;
}

CppCompilator::CppCompilator(uint8_t* argfilebuffer, uint64_t argfilelength) :
	m_file_buffer(argfilebuffer), 
	m_file_length(argfilelength), 

	m_measure_error_signal(false),
	m_data_size_offset(0),
	m_actual_level_index(-1),

	m_its_string(false), 
	m_its_comment(false),
	m_instruction(string()), 
	m_value_type(string()), 
	m_name(string()), 
	m_value(string())
{
	InitializeHashMap();

	if (m_file_buffer != nullptr && m_file_length != 0)
	{
		DeleteComments();
		DeleteContrCharAndPutToVector();
	}

}


void CppCompilator::DeleteComments()
{
	for (uint64_t i = 0; i < m_file_length - 1; ++i)
	{
		if ((m_file_buffer[i] == '/') && (m_file_buffer[i + 1] == '/'))
		{
			m_its_comment = !(m_its_comment);
		}

		if ((m_file_buffer[i] == 0x0D) && (m_file_buffer[i + 1] == 0x0A) && (m_its_comment == true))
		{
			m_its_comment = !(m_its_comment);
		}

		if (m_its_comment == true)
		{
			m_file_buffer[i] = ' ';
			continue;
		}
	}

}


void CppCompilator::DeleteContrCharAndPutToVector()
{
	for (uint64_t i = 0; i < m_file_length - 1; ++i)
	{
		if (m_file_buffer[i] == '\"')
		{
			m_its_string = !m_its_string;
		}

		if ((m_file_buffer[i] <= 32) && (m_its_string == false))
		{
			continue;
		}

		m_clean_file.push_back(m_file_buffer[i]);
	}
}

void CppCompilator::GetValuesFromString()
{
	while (1)
	{

		bool nopswitch = true;

		//Sprawdzenie czy jest prawidlowy typ
		for (uint32_t i = 0; i < value_types.size(); ++i)
		{
			if (m_instruction.substr(0, string(value_types[i]).length()) == value_types[i])
			{
				m_value_type = value_types[i];
				nopswitch = false;
				break;
			}
		}

		// Jesli typ byl nieprawidlowy czyli nopswitch == true to wychodze
		if (nopswitch == true)
		{
			m_value_type = "nop";
			m_name = string();
			m_value = string();
			return;
		}

		//Wyodrebnienie nazwy od instrukcji
		for (uint64_t i = m_value_type.length(); i < m_instruction.length(); ++i)
		{
			if ((m_instruction[i] == '=') || (m_instruction[i] == '['))
			{
				if (m_instruction[i] == '[')
				{
					m_value_type += "[]";
				}

				break;
			}

			m_name += m_instruction[i];
		}

		//Wyodrebnienie wartosci od instrukcji
		uint64_t valueindex = m_instruction.find("=") + 1;
		m_value = m_instruction.substr(valueindex, m_instruction.length() - valueindex - 1);


		// Typ string lub tablica char[] lub GUID lub samo char
		if ((m_value_type == "string") || (m_value_type == "char[]") || (m_value_type == "GUID") || (m_value_type == "char"))
		{
			m_value = m_instruction.substr(valueindex + 1, m_instruction.length() - (valueindex + 1) - 2);
			return;
		}

		string temp_name_uppercased = Misc::ToUppercase(m_name);

		for (int i = 0; i < this->c_number_of_levels; ++i)
		{
			const string temp_level_string = ::level_struct_name + "_" + to_string(i);
			
			if (m_value_type == "struct" && temp_name_uppercased.substr(0, temp_level_string.length()) == temp_level_string)
			{
				DEBUG_PRINT("LEVEL ");
				DEBUG_PRINT(to_string(i).c_str());
				DEBUG_PRINT(" DETECTED!\n");

				this->m_actual_level_index = i;
			}
		}

		// Typ struktury
		if (m_value_type == "struct" || m_value_type == "namespace")
		{

			DEBUG_PRINT("TEMP UPPERCASED = |"); DEBUG_PRINT(temp_name_uppercased.c_str());  DEBUG_PRINT("\n");
			DEBUG_PRINT("::wave_struct_name = |"); DEBUG_PRINT(::wave_struct_name.c_str()); DEBUG_PRINT("|\n");

			if (temp_name_uppercased.substr(0, ::wave_struct_name.length()) == ::wave_struct_name)
			{
				++this->m_arr_of_number_of_waves[this->m_actual_level_index];
			}

			valueindex = m_instruction.find("{") + 1;
			m_instruction = m_instruction.substr(valueindex, m_instruction.length() - valueindex);

			m_value_type = string();
			m_name = string();
			m_value = string();

			continue;
		}


		// measure_all_value_sizes_after_this_instruction();
		if (
			m_value_type == "uint32_t" 
			&& 
			m_name == "data_size" 
			&& 
			m_value == ::measure_all_value_sizes_after_this_instruction_function_name
			)
		{
			m_value = "0";
			this->m_measure_error_signal = true;
			this->m_data_size_offset = this->output_file_buffer.size();
			return;
		}

		for (int i = 0 ; i < this->c_number_of_levels ; ++i)
		{
			if (
				m_value_type == "uint32_t" 
				&& 
				m_name == ("number_of_waves_in_level_" + to_string(i)) 
				&& 
				m_value == ::count_function_name
				)
			{
				m_value = "0";
				this->m_arr_of_signals_for_levels[i] = true;
				this->m_arr_of_offsets_for_number_of_waves[i] = this->output_file_buffer.size();
				return;
			}
		}

		// Typ boolean
		if (m_value_type == "bool")
		{
			if (m_value == "true")
			{
				m_value = "1";
			}
			if (m_value == "false")
			{
				m_value = "0";
			}
			return;
		}


		break;
	}

}

void CppCompilator::WriteGUID(string& slowo)
{
	const int hex_base = 16;

	uint32_t four_bytes = stoul(slowo.substr(0, 8), nullptr, hex_base);
	WriteRaw<uint32_t>(output_file_buffer, four_bytes);

	uint16_t two_bytes = static_cast<uint16_t>(stoul(slowo.substr(9, 4), nullptr, hex_base));
	WriteRaw<uint16_t>(output_file_buffer, two_bytes);

	two_bytes = static_cast<uint16_t>(stoul(slowo.substr(14, 4), nullptr, hex_base));
	WriteRaw<uint16_t>(output_file_buffer, two_bytes);

	two_bytes = static_cast<uint16_t>(_byteswap_ushort(static_cast<unsigned short>(stoul(slowo.substr(19, 4), nullptr, hex_base))));
	WriteRaw<uint16_t>(output_file_buffer, two_bytes);

	four_bytes = static_cast<uint32_t>(_byteswap_ulong(stoul(slowo.substr(24, 8), nullptr, hex_base)));
	WriteRaw<uint32_t>(output_file_buffer, four_bytes);

	two_bytes = static_cast<uint32_t>(_byteswap_ushort(static_cast<unsigned short>(stoul(slowo.substr(32, 4), nullptr, hex_base))));
	WriteRaw<uint16_t>(output_file_buffer, two_bytes);

}


void CppCompilator::WriteString(string& str)
{
	output_file_buffer.insert(output_file_buffer.end(), str.begin(), str.end());

	string null_terminator = "0";
	WriteValueToFile<uint8_t>(null_terminator, 'u');
}

template <typename T>
void CppCompilator::WriteValueToFile(string& value, uint8_t mode)
{
	T typevalue;

	switch (mode)
	{
	case 'u':
		typevalue = static_cast<T>(stoul(value));
		break;

	case 'd':
		typevalue = static_cast<T>(stol(value));
		break;

	default: break;
	}

	WriteRaw<T>(output_file_buffer, typevalue);

}



template <typename T>
void CppCompilator::WriteTabToFile(string& str, uint8_t mode)
{
	T writedvalue = 0;

	if (m_value == "{}") return;

	string scanedvalue = string();
	for (uint64_t i = 0; i < str.length(); ++i)
	{
		if (str[i] == '{') continue;


		if ((str[i] == ',') || (str[i] == '}'))
		{

			switch (mode)
			{

			case 'd': writedvalue = static_cast<T>(stol(scanedvalue));  break;
			case 'u': writedvalue = static_cast<T>(stoul(scanedvalue)); break;
			default: break;
			}

			WriteRaw<T>(output_file_buffer, writedvalue);

			scanedvalue = string();
			continue;
		}

		scanedvalue += str[i];
	}
}




uint64_t CppCompilator::CountValuesInTab(string& value)
{
	uint64_t tabsize = 0;

	for (uint64_t i = 0; i < value.length(); ++i)
	{
		if (value[i] == ',')
			++tabsize;
	}

	if (tabsize != 0) ++tabsize;

	return tabsize;
}


void CppCompilator::WriteInstructionToBin(string& valuetype, string& name, string& value)
{

	// uints
	if (valuetype.substr(0, 4) == "uint")
	{
		// uint32_t
		if (valuetype == value_types[6])
		{
			WriteValueToFile<uint32_t>(value, 'u');
			return;
		}

		// uint8_t
		if (valuetype == value_types[2])
		{
			output_file_buffer.push_back(static_cast<uint8_t>(stoul(value)));
			return;
		}

		// uint16_t
		if (valuetype == value_types[4])
		{
			WriteValueToFile<uint16_t>(value, 'u');
			return;
		}

		// uint64_t
		if (valuetype == value_types[8])
		{
			WriteValueToFile<uint64_t>(value, 'u');
			return;
		}

		// uint32_t[]
		if (valuetype == value_types[7])
		{
			WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

			WriteTabToFile<uint32_t>(value, 'u');

			return;
		}


		// uint8_t[]
		if (valuetype == value_types[3])
		{
			WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

			WriteTabToFile<uint8_t>(value, 'u');

			return;
		}

		// uint16_t[]
		if (valuetype == value_types[5])
		{
			WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

			WriteTabToFile<uint16_t>(value, 'u');

			return;
		}

		// uint64_t[]
		if (valuetype == value_types[9])
		{
			WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

			WriteTabToFile<uint64_t>(value, 'u');

			return;
		}
	}

	// string
	if (valuetype == value_types[11])
	{
		WriteString(value);
		return;
	}

	// 32bituniquekey
	if (valuetype == value_types[29])
	{
		WriteRaw<uint32_t>(output_file_buffer, m_list_of_consts[value]);
		return;
	}

	// bool
	if (valuetype == value_types[27])
	{
		output_file_buffer.push_back(static_cast<int8_t>(stol(value)));
		return;
	}

	// bool[]
	if (valuetype == value_types[28])
	{

		WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));
		WriteTabToFile<int8_t>(value, 'd');

		return;
	}


	if (valuetype.substr(0, 3) == "int")
	{

		// int32_t albo int
		if ((valuetype == value_types[17]) || (valuetype == value_types[23]))
		{
			WriteValueToFile<int32_t>(value, 'd');
			return;
		}

		// int8_t
		if (valuetype == value_types[13])
		{
			output_file_buffer.push_back(static_cast<int8_t>(stol(value)));
			return;
		}

		// int16_t 
		if (valuetype == value_types[15])
		{
			WriteValueToFile<int16_t>(value, 'd');
			return;
		}

		// int64_t 
		if (valuetype == value_types[19])
		{
			WriteValueToFile<int64_t>(value, 'd');
			return;
		}


		// int32_t[] albo int[]
		if ((valuetype == value_types[18]) || (valuetype == value_types[24]))
		{
			WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

			WriteTabToFile<int32_t>(value, 'd');

			return;
		}

		// int8_t[]
		if (valuetype == value_types[14])
		{
			WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

			WriteTabToFile<int8_t>(value, 'd');

			return;
		}


		// int16_t[] 
		if (valuetype == value_types[16])
		{
			WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

			WriteTabToFile<int16_t>(value, 'd');

			return;
		}


		// int64_t[] 
		if (valuetype == value_types[20])
		{
			WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

			WriteTabToFile<int64_t>(value, 'd');

			return;
		}

	}

	//  char || char[]
	if ((valuetype == value_types[0]) || (valuetype == value_types[1]))
	{
		output_file_buffer.insert(output_file_buffer.end(), value.begin(), value.end());
		return;
	}

	// short
	if (valuetype == value_types[21])
	{
		WriteValueToFile<int16_t>(value, 'd');
		return;
	}

	// short[]
	if (valuetype == value_types[22])
	{
		WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

		WriteTabToFile<int16_t>(value, 'd');

		return;
	}

	// long int
	if (valuetype == value_types[25])
	{
		WriteValueToFile<int64_t>(value, 'd');
		return;
	}

	// long int[]
	if (valuetype == value_types[26])
	{
		WriteRaw<uint32_t>(output_file_buffer, static_cast<uint32_t>(CountValuesInTab(value)));

		WriteTabToFile<int64_t>(value, 'd');

		return;
	}


	// GUID
	if (valuetype == value_types[10])
	{
		WriteGUID(value);
		return;
	}


}


//Cpp
void CppCompilator::CompileCppFileToBin()
{
	for (uint64_t i = 0; i < m_clean_file.size(); ++i)
	{
		m_instruction += m_clean_file[i];

		if (m_clean_file[i] == ';')
		{
			DEBUG_PRINT("----\n");
			DEBUG_PRINT(m_instruction.c_str());
			DEBUG_PRINT("\n");

			GetValuesFromString();
			WriteInstructionToBin(m_value_type, m_name, m_value);

			DEBUG_PRINT(m_value_type.c_str()); DEBUG_PRINT("\n");
			DEBUG_PRINT(m_name.c_str()); DEBUG_PRINT("\n");
			DEBUG_PRINT(m_value.c_str()); DEBUG_PRINT("\n");
			DEBUG_PRINT("----\n");

			m_instruction = string();
			m_value_type = string();
			m_name = string();
			m_value = string();
		}
	}

	//Poprawka dla measure_all_value_sizes_after_this_instruction();
	if (this->m_measure_error_signal == true)
	{
		uint32_t data_size = static_cast<uint32_t>(
							 this->output_file_buffer.size() - (this->m_data_size_offset + sizeof(data_size))
						     );

		memcpy(&output_file_buffer[m_data_size_offset], &data_size, sizeof(data_size));

		this->m_measure_error_signal = false;
	}

	for (int j = 0; j < this->c_number_of_levels; ++j)
	{
		if (this->m_arr_of_signals_for_levels[j] == true)
		{
			memcpy(	&output_file_buffer[ m_arr_of_offsets_for_number_of_waves[j] ], 
					&m_arr_of_number_of_waves[j],
					sizeof(m_arr_of_number_of_waves[j]));

			this->m_arr_of_signals_for_levels[j] = false;
		}
	}

}

const vector<char>& CppCompilator::GetOutputFileBuffer() const
{
	return this->output_file_buffer;
}