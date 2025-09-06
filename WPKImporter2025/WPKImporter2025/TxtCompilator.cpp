#include "WpkImporter.hpp"


//------------------
// Klasa Txt Compilator
//------------------

TxtCompilator::TxtCompilator(uint8_t* argfilebuffer, uint64_t argfilelength, ostream& arg_o) :
	m_file_buffer(argfilebuffer),
	m_file_length(argfilelength),
	m_o(arg_o),

	m_instruction(string()),
	m_obj_name(string()),
	m_inside_obj_type(string()),
	m_inside_obj_name(string()),
	bracket_counter(::initialize_signal_value),
	m_line_number(0),

	m_its_comment(false)
{
	DeleteComments();
	CleanFileBuffer(m_clean_file, true);
}

void TxtCompilator::DeleteComments()
{
	for (uint64_t i = 0; i < m_file_length; ++i)
	{
		if ((m_file_buffer[i] == '/') && (m_file_buffer[i + 1] == '/'))
		{
			m_its_comment = !(m_its_comment);
		}

		if ( (m_file_buffer[i] == '\n') && (m_its_comment == true))
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


void TxtCompilator::CleanFileBuffer(vector<char>& arg_clean_file, bool change_newline_switch)
{
	bool its_space = false;

	for (size_t i = 0; i < m_file_length; ++i)
	{

		if (m_file_buffer[i] == ' ' && its_space == false)
		{
			arg_clean_file.push_back(m_file_buffer[i]);
			its_space = true;
			continue;
		}

		if (m_file_buffer[i] != ' ')
		{
			its_space = false;
		}

		if (its_space == true)
		{
			continue;
		}

		if (m_file_buffer[i] == '\t')
		{
			continue;
		}

		if ((m_file_buffer[i] == '\n') && (change_newline_switch == true))
		{
			arg_clean_file.push_back(';');
			continue;
		}

		arg_clean_file.push_back(m_file_buffer[i]);
	}


}

// txt
void TxtCompilator::CompileTxtFileToBinBuffers()
{

	for (uint64_t i = 0; i < m_clean_file.size(); ++i)
	{
		m_instruction += m_clean_file[i];

		if (m_clean_file[i] == ';')
		{
			++this->m_line_number;
			m_instruction.pop_back();

			TU::PrepareString(m_instruction, "{");
			TU::PrepareString(m_instruction, "}");

			DEBUG_PRINT("----\n\"");
			DEBUG_PRINT(m_instruction.c_str());
			DEBUG_PRINT("\"\n");

			string uppercased_instruction = Misc::ToUppercase(m_instruction);
			InterpretTxtWavePacksInstructions(uppercased_instruction, m_instruction);

			DEBUG_PRINT("----\n");

			m_instruction = string();
		}
	}


}


void TxtCompilator::InterpretTxtWavePacksInstructions(
	string& uppercased_instruction, 
	string& original_instruction
)
{
	TU::GetObjType(
		uppercased_instruction, 
		original_instruction, 
		m_inside_obj_type, 
		m_inside_obj_name, 
		::wavepack_namespace_name_with_space);

	if (m_inside_obj_type == ::wavepack_namespace_name)
	{
		DEBUG_PRINT("ITS WAVEPACK!!!\n");

		CompileObject<WavePack>(uppercased_instruction,
								original_instruction,
								m_whole_particle_emiter_container.m_single_wavepack,
								m_whole_particle_emiter_container.m_wavepacks);
	}
}

void TxtCompilator::SaveOutputFileBuffersToBinFiles(fs::path& arg_input_path)
{
	m_o << endl
		<< "Created wavepack output files:" << endl
		<< "------------------------------" << endl;

	size_t i = 1;

	for (auto& single_wavepack : m_whole_particle_emiter_container.m_wavepacks)
	{

		vector<char> temp_output_file_buff = {};
		single_wavepack.WriteToFile(temp_output_file_buff);
		
		string wavepack_name = single_wavepack.GetWavePackName();

		string output_bin_file_name = (arg_input_path.parent_path() / (wavepack_name + ::wpk_format)).string();

		m_o << i << ". " << output_bin_file_name << endl;

		BW::SaveOutputFileBufferToBinFile(output_bin_file_name, temp_output_file_buff);

		++i;
	}
	
}