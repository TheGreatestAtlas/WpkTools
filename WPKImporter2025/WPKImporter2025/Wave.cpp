#include "WpkImporter.hpp"

Wave::Wave(uint32_t arg_wavepack_type) :
    m_wave_path(string()),
    m_wavepack_type(arg_wavepack_type),

    m_error(false),
    m_line_number(0)
{
}

Wave::~Wave()
{
}

size_t Wave::GetLineNumber() const { return this->m_line_number;  }

uint32_t Wave::GetSizeOf()
{
    uint32_t result = 0;

    if (this->m_wavepack_type == ::new_wpk_format_type)
    {
        result = sizeof(this->m_a) +
                 sizeof(this->m_b) +
                 sizeof(this->m_c) +
                 static_cast<uint32_t>(this->m_wave_path.length()) +
                 sizeof(char); // \0
    }


    if (this->m_wavepack_type == ::old_wpk_format_type)
    {
        result = sizeof(this->m_a[0]) +
                 static_cast<uint32_t>(this->m_wave_path.length()) +
                 sizeof(char); // \0
    }

    DEBUG_PRINT("SIZE OF WAVE = ");
    DEBUG_PRINT(to_string(result).c_str());
    DEBUG_PRINT("\n");

    return result;
}

void Wave::GetFromFile(string& uppercased_instruction, string& original_instruction, size_t& line_number)
{
    const size_t wave_struct_name_with_space_len = ::wave_struct_name_with_space.length();

    if (uppercased_instruction.substr(0, wave_struct_name_with_space_len) == ::wave_struct_name_with_space)
    {
        this->m_line_number = line_number;

        string m_help_str = 
        original_instruction.substr
        (
            wave_struct_name_with_space_len,
            original_instruction.length() - wave_struct_name_with_space_len
        );

        const unsigned int size_of_c_help = 255;
        char c_help[size_of_c_help] = { '\0' };

        int32_t temp_values[this->a_size + this->b_size + this->c_size] = { 0 };

        DEBUG_PRINT("WAVE TYPE = ");
        DEBUG_PRINT(to_string(this->m_wavepack_type).c_str());
        DEBUG_PRINT("\n");

        if (this->m_wavepack_type == ::new_wpk_format_type)
        {
            if( sscanf_s
                (
                m_help_str.c_str(),
                "%d %d %d %d %d %d delayed %s",
                &temp_values[0],
                &temp_values[1],
                &temp_values[2],
                &temp_values[3],
                &temp_values[4],
                &temp_values[5],
                c_help,
                size_of_c_help
                ) < 7
            )
            {
                if (sscanf_s
                    (
                    m_help_str.c_str(),
                    "%d %d %d %d %d %d %s",
                    &temp_values[0],
                    &temp_values[1],
                    &temp_values[2],
                    &temp_values[3],
                    &temp_values[4],
                    &temp_values[5],
                    c_help,
                    size_of_c_help
                    ) < 7
                )
                {
                    this->m_error = true;
                }
            }
            else
            {
                temp_values[6] = ::delayed_const_value;
            }
        }


        if (this->m_wavepack_type == ::old_wpk_format_type)
        {
            if(sscanf_s
                (
                m_help_str.c_str(), 
                "%d %s", 
                &temp_values[0], 
                c_help, 
                size_of_c_help
                ) < 2
            )
            {
                this->m_error = true;
            }
        }

        for (int i = 0; i < _countof(c_help); ++i)
        {
            if (c_help[i] == NULL) { break; }
            this->m_wave_path += c_help[i];
        }

        for(int i = 0 ; i < this->a_size; ++i)
            this->m_a[i] = static_cast<uint32_t>(temp_values[i]);

        for(int i = 0 ; i < this->b_size; ++i)
            this->m_b[i] = static_cast<uint8_t>(temp_values[i + this->a_size]);
   
        for(int i = 0 ; i < this->c_size; ++i)
            this->m_c[i] = static_cast<uint32_t>(temp_values[i + this->a_size + this->b_size]);


        DEBUG_PRINT("WAVE:\n");
        DEBUG_PRINT("{\n");
        DEBUG_PRINT(to_string(this->m_a[0]).c_str()); DEBUG_PRINT("\n");
        DEBUG_PRINT(to_string(this->m_a[1]).c_str()); DEBUG_PRINT("\n");
        DEBUG_PRINT(to_string(this->m_a[2]).c_str()); DEBUG_PRINT("\n");
        DEBUG_PRINT(to_string(this->m_b[0]).c_str()); DEBUG_PRINT("\n");
        DEBUG_PRINT(to_string(this->m_b[1]).c_str()); DEBUG_PRINT("\n");
        DEBUG_PRINT(to_string(this->m_c[0]).c_str()); DEBUG_PRINT("\n");
        DEBUG_PRINT(to_string(this->m_c[1]).c_str()); DEBUG_PRINT("\n");
        DEBUG_PRINT(this->m_wave_path.c_str()); DEBUG_PRINT("\n");
        DEBUG_PRINT("}\n\n");
    }

}


bool Wave::GetError() const { return this->m_error; }

void Wave::WriteToFile(vector<char>& output_file_buff)
{
    switch (this->m_wavepack_type)
    {
    case old_wpk_format_type:
        BW::WriteValue(output_file_buff, this->m_a[0]);
        BW::WriteString(output_file_buff, this->m_wave_path);
    break;

    case new_wpk_format_type:
        BW::WriteValue(output_file_buff, this->m_a);
        BW::WriteValue(output_file_buff, this->m_b);
        BW::WriteValue(output_file_buff, this->m_c);
        BW::WriteString(output_file_buff, this->m_wave_path);
    break;

    default:
    break;
    }
}
