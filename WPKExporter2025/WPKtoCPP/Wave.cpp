#include "WpkExporter.hpp"

Wave::Wave(uint32_t arg_wavepack_type) :
    m_wave_path(string()), 
    m_wavepack_type(arg_wavepack_type)
{}

Wave::~Wave()
{}

void Wave::GetFromFile(uint8_t* buff, size_t& curr_offset)
{
    switch (this->m_wavepack_type)
    {
    case old_wpk_format_type:
        this->m_a[0] = BR::GetBinVal<uint32_t>(buff, curr_offset);
        this->m_wave_path = BR::GetString(buff, curr_offset);
    break;

    case new_wpk_format_type:
        BR::GetBinTab<uint32_t>(this->m_a, this->a_size, buff, curr_offset);
        BR::GetBinTab<uint8_t>(this->m_b, this->b_size, buff, curr_offset);
        BR::GetBinTab<uint32_t>(this->m_c, this->c_size, buff, curr_offset);
        this->m_wave_path = BR::GetString(buff, curr_offset);
    break;

    default:
    break;
    }
}

string Wave::ToOldFormatString() const
{
    stringstream ss;

    switch (this->m_wavepack_type)
    {
    case old_wpk_format_type:

        ss  << ::wave_struct_name
            << " "
            << this->m_a[0]
            << " "
            << this->m_wave_path;
    break;

    case new_wpk_format_type:

         ss << ::wave_struct_name
            << " "
            << this->m_a[0]
            << " "
            << this->m_a[1]
            << " "
            << this->m_a[2]
            << " "
            << static_cast<uint32_t>(this->m_b[0])
            << " "
            << static_cast<uint32_t>(this->m_b[1])
            << " "
            << this->m_c[0] 
            << " ";

        if (this->m_c[1] == ::delayed_const_value)
        {
            ss << ::delayed_const_string << " ";
        }

        ss << this->m_wave_path;

    break;

    default:
    break;
    }

    return ss.str();
}

string Wave::ToNewFormatString() const
{
    stringstream ss;

    switch (this->m_wavepack_type)
    {
    case old_wpk_format_type:

        ss << ::struct_type << " " << ::wave_struct_name << "{ ";

        CW::WriteTrivialValue(ss, "a_0", this->m_a[0]);
        ss << "\t";

        ss << ::string_type << " " << ::wave_path_string_name << " = \"" << this->m_wave_path << "\"; };";

    break;

    case new_wpk_format_type:

        ss << ::struct_type << " " << ::wave_struct_name << "{ ";

        for (int i = 0; i < this->a_size; ++i)
        {
            CW::WriteTrivialValue(ss, "a_" + to_string(i), this->m_a[i]);
            ss << "\t";
        }

        for (int i = 0; i < this->b_size; ++i)
        {
            CW::WriteTrivialValue(ss, "b_" + to_string(i), this->m_b[i]);
            ss << "\t";
        }

        for (int i = 0; i < this->c_size; ++i)
        {
            if (i == 1)
            {
                ss << ::bit_32_unique_key_type << " " << "c_" << i << " = ";


                switch (this->m_c[i])
                {
                    case ::null_const_string_value:
                        ss << ::null_const_string_uppercase;
                    break;

                    case ::delayed_const_value:
                        ss << ::delayed_const_string_uppercase;
                    break;

                    default:
                        ss << this->m_c[i];
                    break;
                }

                ss << ";\t";

                continue;
            }

            CW::WriteTrivialValue(ss, "c_" + to_string(i), this->m_c[i]);
            ss << "\t";
        }

        ss << ::string_type << " " << ::wave_path_string_name << " = \"" << this->m_wave_path << "\"; };\t   ";
    break;

    default:
    break;
    }

    return ss.str();
}