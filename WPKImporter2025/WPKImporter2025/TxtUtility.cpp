#include "WpkImporter.hpp"

void TxtUtility::MoveByLines(stringstream& arg_stream, int count)
{
    string null_str;

    for (int i = 0; i < count; ++i)
        getline(arg_stream, null_str);
}


bool TxtUtility::GetSingleString(string& arg_line, string& instruction_name, string& dst)
{
    instruction_name += " ";

    const size_t intruction_name_len = instruction_name.length();

    if (arg_line.substr(0, intruction_name_len) == instruction_name)
    {
        dst = arg_line.substr(intruction_name_len, arg_line.length() - intruction_name_len);

        DEBUG_PRINT("STR = ");
        DEBUG_PRINT(dst.c_str());
        DEBUG_PRINT("\n");
        return ::success_value;
    }

    return ::failure_value;
}

bool TxtUtility::GetObjType(string& uppercased_instruction, 
                            string& original_instruction,
                            string& inside_obj_type, 
                            string& inside_obj_name, 
                            string obj_type_name)
{

    if (uppercased_instruction.substr(0, obj_type_name.length()) == obj_type_name)
    {
        inside_obj_type = original_instruction.substr(0, obj_type_name.length() - 1);

        TU::GetSingleString(original_instruction, inside_obj_type, inside_obj_name);

        inside_obj_type = obj_type_name;
        inside_obj_type.pop_back();


        DEBUG_PRINT("m_inside_obj_type = |");
        DEBUG_PRINT(inside_obj_type.c_str());
        DEBUG_PRINT("|\n");

        DEBUG_PRINT("m_inside_obj_name = |");
        DEBUG_PRINT(inside_obj_name.c_str());
        DEBUG_PRINT("|\n");

        return ::success_value;
    }


    return ::failure_value;
}


bool TxtUtility::GetLevelHeader(string& arg_line,
                                uint32_t& arg_level_index,
                                uint32_t arg_number_of_waves_in_level_checksum[],
                                bool& arg_level_mode)
{
    const uint32_t number_of_levels = 3;

    for (uint32_t i = 0; i < number_of_levels; ++i)
    {
        string temp_level_type_name = ::level_struct_name + to_string(i);
        const size_t temp_level_type_name_len = temp_level_type_name.length();

        if (arg_line.substr(0, temp_level_type_name_len) == temp_level_type_name)
        {
            DEBUG_PRINT("ITS LEVEL HEADER!\n");

            string m_help_str = arg_line.substr(temp_level_type_name_len, arg_line.length() - temp_level_type_name_len);

            sscanf_s
            (
                m_help_str.c_str(),
                "%d",
                &arg_number_of_waves_in_level_checksum[i]
            );

            DEBUG_PRINT("arg_number_of_waves_in_level_checksum = ");
            DEBUG_PRINT(to_string(arg_number_of_waves_in_level_checksum[i]).c_str());
            DEBUG_PRINT("\n");

            arg_level_index = i;

            arg_level_mode = true;

            return ::success_value;
        }
    }

    return ::failure_value;
}


void TxtUtility::GetWaves(string& uppercased_instruction,
                          string& original_instruction,
                          uint32_t& arg_wavepack_type,
                          vector<Wave>& arg_wave_vector_level,
                          uint32_t& arg_number_of_waves_in_level_checksum,
                          bool& arg_level_mode,
                          size_t& line_number)
{
    if (uppercased_instruction.substr(0, ::level_struct_name.length()) != ::level_struct_name &&
        uppercased_instruction != "{" &&
        uppercased_instruction != "}" &&
        arg_number_of_waves_in_level_checksum > 0 &&
        arg_level_mode == true)
    {
        if (uppercased_instruction.find(" ") != string::npos)
        {
            DEBUG_PRINT("TU::GetWaves\n");

            Wave temporary_single_wave(arg_wavepack_type);
            temporary_single_wave.GetFromFile(uppercased_instruction, original_instruction, line_number);

            arg_wave_vector_level.push_back(temporary_single_wave);


            --arg_number_of_waves_in_level_checksum;
        }
    }

    if (arg_level_mode == true && arg_number_of_waves_in_level_checksum == 0)
    {
        arg_level_mode = false;
    }

}


bool TxtUtility::CheckWaves(string& uppercased_instruction,
                            uint32_t& arg_number_of_waves_in_level_checksum,
                            bool& arg_level_mode) 
    // success = everything is okay 
    // failure = error detected
{
    if (uppercased_instruction.substr(0, ::wave_struct_name.length()) == ::wave_struct_name &&
        uppercased_instruction != "{" &&
        uppercased_instruction != "}" &&
        arg_number_of_waves_in_level_checksum == 0 &&
        arg_level_mode == false)
    {
        return ::failure_value;
    }


    if (uppercased_instruction == "}" &&
        arg_number_of_waves_in_level_checksum > 0 &&
        arg_level_mode == true)
    {
        return ::failure_value;
    }

    return ::success_value;
}
