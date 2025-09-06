#include "WpkExporter.hpp"

WavePack::WavePack(string arg_wavepack_name):
	m_wavepack_name(arg_wavepack_name),
    m_wavepack_type(0),
	m_data_size(0),

    m_special_hh_and_ww3_case(false)
{}

WavePack::~WavePack()
{}

void WavePack::GetWaveVector(vector<Wave>& arg_wave_vector, 
                             uint32_t& arg_number_of_waves, 
                             uint8_t* buff, 
                             size_t& curr_offset)
{
   
    if (arg_number_of_waves != 0)
    {
        for (uint32_t j = 0; j < arg_number_of_waves; ++j)
        {
            Wave single_wave(this->m_wavepack_type);
            single_wave.GetFromFile(buff, curr_offset);
            arg_wave_vector.push_back(move(single_wave));
        }
    }
}

void WavePack::WriteWaveVector(vector<Wave>& arg_wave_vector,
    uint32_t& arg_number_of_waves,
    uint32_t& arg_level_number,
    stringstream& arg_ss,
    StringFormatMode arg_string_format_mode)
{
    
    if (arg_number_of_waves != 0)
    {
        switch (arg_string_format_mode)
        {
        case OldStringFormat:
            arg_ss << "\t" << ::level_struct_name << arg_level_number << " " << arg_number_of_waves << endl
                << "\t{" << endl;
        break;

        case NewStringFormat:
            arg_ss << "\t" << ::struct_type << " " << ::level_struct_name << "_" << arg_level_number << endl
                   << "\t{" << endl;
        break;

        default:
        break;
        }

        for (const auto& single_wave : arg_wave_vector)
        {
            
            arg_ss << "\t\t";
            
            switch (arg_string_format_mode)
            {
                case OldStringFormat:
                    arg_ss << single_wave.ToOldFormatString();
                break;

                case NewStringFormat:
                    arg_ss << single_wave.ToNewFormatString();
                break;

                default:
                break;
            }

            arg_ss << endl;
        }


        switch (arg_string_format_mode)
        {
        case OldStringFormat:
            arg_ss << "\t}" << endl;
        break;

        case NewStringFormat:
            arg_ss << "\t};" << endl;
        break;

        default:
        break;
        }
        
    }
}

void WavePack::GetFromFile(uint8_t* buff, size_t& curr_offset, size_t size_of_file)
{
    BR::GetBinTab<uint8_t>(this->m_format, this->format_size, buff, curr_offset);

    this->m_wavepack_type = BR::GetBinVal<uint32_t>(buff, curr_offset);

    this->m_number_of_waves_in_level[0] = BR::GetBinVal<uint32_t>(buff, curr_offset);
    this->m_number_of_waves_in_level[1] = BR::GetBinVal<uint32_t>(buff, curr_offset);

    if (this->m_wavepack_type == ::old_wpk_format_type)
    {
        uint32_t temp_data_size_value = BR::GetBinVal<uint32_t>(buff, curr_offset);

        if (temp_data_size_value == size_of_file - curr_offset)
        {
            this->m_special_hh_and_ww3_case = true;
        }

        curr_offset -= sizeof(temp_data_size_value);
    }

    if(this->m_wavepack_type == ::old_wpk_format_type && this->m_special_hh_and_ww3_case == false)
    {
        this->m_number_of_waves_in_level[2] = BR::GetBinVal<uint32_t>(buff, curr_offset);
    }

    this->m_data_size = BR::GetBinVal<uint32_t>(buff, curr_offset);


    for(uint32_t i = 0 ; i < this->c_number_of_levels ; ++i)
        GetWaveVector(this->m_wave_vector_level[i], this->m_number_of_waves_in_level[i], buff, curr_offset);
}


string WavePack::ToOldFormatString()
{
    stringstream ss;

    ss << ::wavepack_namespace_name << " " << this->m_wavepack_name << endl
       << "{" << endl;

    if(this->m_wavepack_type == ::old_wpk_format_type)
    {
        ss << "\t" << ::type_value_name << " " << this->m_wavepack_type << endl;
        ss << "\t" << ::disable_level_2_value_name << " " << this->m_special_hh_and_ww3_case << endl;
    }

    for(uint32_t i = 0 ; i < this->c_number_of_levels; ++i)
        WriteWaveVector(this->m_wave_vector_level[i], this->m_number_of_waves_in_level[i], i, ss, OldStringFormat);
    

    ss << "}" << endl;

    return ss.str();
}


string WavePack::ToNewFormatString()
{
    stringstream ss;

    for (int i = 0; i < format_size; ++i)
    {
        CW::WriteTrivialValue(ss, ::format_value_name + to_string(i), this->m_format[i]);

        if (isalpha(this->m_format[i]))
            ss << " // " << this->m_format[i];

        ss << endl;
    }

    ss << endl;

    CW::WriteTrivialValue(ss, ::wavepack_type_value_name, this->m_wavepack_type); ss << endl;

    uint32_t temp_number_of_levels = 0;

    if (this->m_wavepack_type == ::old_wpk_format_type && this->m_special_hh_and_ww3_case == false)
    {
        temp_number_of_levels = this->c_number_of_levels;
    }
    else
    {
        temp_number_of_levels = this->c_number_of_levels - 1;
    }

    for (uint32_t i = 0; i < temp_number_of_levels; ++i)
    {
        ss << CW::ConvertTrivialTypeToString(this->m_number_of_waves_in_level[i])
            << " " << ::number_of_waves_in_level_value_name << i << " = " << ::count_function_name << ";"
            << endl;
    }

    ss  << endl
        << CW::ConvertTrivialTypeToString(this->m_data_size) 
        << " " 
        << ::data_size_value_name 
        << " = " 
        << ::measure_all_value_sizes_after_this_instruction_function_name 
        << ";"
        << endl 
        << endl
        
        << ::namespace_type << " " << m_wavepack_name << endl
        << "{" << endl;


    for(uint32_t i = 0 ; i < temp_number_of_levels; ++i)
        WriteWaveVector(this->m_wave_vector_level[i], this->m_number_of_waves_in_level[i], i, ss, NewStringFormat);
    
    ss << "};" << endl;

    return ss.str();
}

void WavePack::ShowInformations(ostream& arg_o) const
{
    uint32_t temp_number_of_levels = 0;

    if (this->m_wavepack_type == ::old_wpk_format_type && this->m_special_hh_and_ww3_case == false)
    {
        temp_number_of_levels = this->c_number_of_levels;
    }
    else
    {
        temp_number_of_levels = this->c_number_of_levels - 1;
    }


    arg_o   << "Format: " << string(reinterpret_cast<const char*>(this->m_format)) << endl
            << "Wavepack type: " << this->m_wavepack_type << endl
            << "Size of data: " << this->m_data_size << endl;


    for (uint32_t i = 0; i < temp_number_of_levels; ++i)
        arg_o << "Number of waves in level" << i << ": " << this->m_number_of_waves_in_level[i] << endl;

}