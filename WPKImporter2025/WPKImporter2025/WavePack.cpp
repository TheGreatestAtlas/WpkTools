#include "WpkImporter.hpp"

WavePack::WavePack(string arg_wavepack_name) :
    m_wavepack_name(arg_wavepack_name),
    m_wavepack_type(::new_wpk_format_type),
    m_data_size(0),

    m_special_hh_and_ww3_case(static_cast<uint32_t>(false)),
    m_level_index(0),
    m_level_mode(false),

    m_type_instruction_line_number(0),
    m_disable_level_2_instruction_line_number(0)
{
}

WavePack::~WavePack()
{
}

string WavePack::GetWavePackName() const { return this->m_wavepack_name; }

void WavePack::GetFromFile(string& uppercased_instruction, string& original_instruction, size_t& line_number)
{
    if (//error detection:
        TU::CheckWaves(
            uppercased_instruction,
            this->m_number_of_waves_in_level_checksum[this->m_level_index],
            this->m_level_mode
        )
        ==
        ::failure_value
        )
    {
        this->m_waves_error[this->m_level_index] = true;
    }

    if (
        TU::GetOneValue<uint32_t>
        (
            uppercased_instruction,
            ::type_instruction_name,
            "%d",
            this->m_wavepack_type
        )
        == 
        ::success_value
        )
    {
        this->m_type_instruction_line_number = line_number;
    }

    if (
        TU::GetOneValue<uint32_t>
        (
            uppercased_instruction,
            ::disable_level_2_instruction_name,
            "%d",
            this->m_special_hh_and_ww3_case
        )
        ==
        ::success_value
        )
    {
        this->m_disable_level_2_instruction_line_number = line_number;
    }


    if (
        TU::GetLevelHeader
        (
            uppercased_instruction,
            this->m_level_index,
            this->m_number_of_waves_in_level_checksum,
            this->m_level_mode
        )
        ==
        ::success_value
        )
    {
        this->m_level_instruction_line_number[this->m_level_index] = line_number;
    }
                        
    TU::GetWaves(uppercased_instruction,
                 original_instruction,
                 this->m_wavepack_type,
                 this->m_wave_vector_level[this->m_level_index],
                 this->m_number_of_waves_in_level_checksum[this->m_level_index],
                 this->m_level_mode,
                 line_number);



}

void WavePack::DebugTheObject(ostream& arg_o)
{
    bool error = false;

    if (this->m_special_hh_and_ww3_case > static_cast<uint32_t>(true))
    {
        arg_o   << endl
                << "-- Error: ----------------------------------------------------------------------------" << endl
                << " The instruction named DisableLevel2 should take the value 0 or 1..."                   << endl
                << " Wavepack name: " << this->m_wavepack_name                                              << endl
                << " Line: " << this->m_disable_level_2_instruction_line_number                             << endl
                << "--------------------------------------------------------------------------------------" << endl
                << endl;

        this->m_special_hh_and_ww3_case = static_cast<uint32_t>(static_cast<bool>(this->m_special_hh_and_ww3_case));

        error = true;
    }

    for (uint32_t i = 0; i < this->c_number_of_levels - this->m_special_hh_and_ww3_case; ++i)
    {
        if (this->m_waves_error[i] == true)
        {
            arg_o   << endl
                    << "-- Error: ----------------------------------------------------------------------------" << endl
                    << " The number of Waves in the Level header does not match the actual number of Waves."    << endl
                    << " Wavepack name: " << this->m_wavepack_name                                              << endl
                    << " Line: " << this->m_level_instruction_line_number[i]                                    << endl
                    << "--------------------------------------------------------------------------------------" << endl
                    << endl;

            error = true;
        }
    }

    for (uint32_t i = 0; i < c_number_of_levels - this->m_special_hh_and_ww3_case; ++i)
    {
        size_t j = 0; 
        for (auto& single_wave : m_wave_vector_level[i])
        {
            if (single_wave.GetError())
            {
                arg_o << endl
                    << "-- Error: ----------------------------------------------------------------------------" << endl
                    << " Wave " << j << " in level " << i <<" has an error!"                                    << endl
                    << " Wavepack name: " << this->m_wavepack_name                                              << endl
                    << " Line: " << single_wave.GetLineNumber()                                                 << endl
                    << "--------------------------------------------------------------------------------------" << endl
                    << endl;

                error = true;
            }
            ++j;
        }
    }

    if (error == false)
    {
        arg_o << this->m_wavepack_name << " ..... OK" << endl;
    }
    else
    {
        arg_o << "The file has errors and cannot be compiled... " << endl;
        exit(EXIT_FAILURE);
    }

}

void WavePack::WriteToFile(vector<char>& output_file_buff)
{
    const uint8_t wpk_format[] = { 'W', 'P', 'K', '\0' };

    BW::WriteValue(output_file_buff, wpk_format);
    BW::WriteValue(output_file_buff, this->m_wavepack_type);

    BW::WriteValue(output_file_buff, static_cast<uint32_t>(this->m_wave_vector_level[0].size()) );
    BW::WriteValue(output_file_buff, static_cast<uint32_t>(this->m_wave_vector_level[1].size()) );

    if ( 
         this->m_wavepack_type == ::old_wpk_format_type 
         && 
         this->m_special_hh_and_ww3_case == static_cast<uint32_t>(false) 
       )
    {
        BW::WriteValue(output_file_buff, static_cast<uint32_t>(this->m_wave_vector_level[2].size()) );
    }

    for (uint32_t i = 0; i < c_number_of_levels - this->m_special_hh_and_ww3_case; ++i)
    {
        for (auto& single_wave : m_wave_vector_level[i])
        {
            this->m_data_size += single_wave.GetSizeOf();
        }
    }

    BW::WriteValue(output_file_buff, this->m_data_size);

    for (uint32_t i = 0; i < c_number_of_levels - this->m_special_hh_and_ww3_case; ++i)
    {
        for (auto& single_wave : m_wave_vector_level[i])
        {
            single_wave.WriteToFile(output_file_buff);
        }
    }
}