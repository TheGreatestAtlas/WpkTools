uint8_t format_0 = 87; // W
uint8_t format_1 = 80; // P
uint8_t format_2 = 75; // K
uint8_t format_3 = 0;

uint32_t wavepack_type = 0;
uint32_t number_of_waves_in_level_0 = count();
uint32_t number_of_waves_in_level_1 = count();

uint32_t data_size = measure_all_value_sizes_after_this_instruction();

namespace EXP_NUCLEAR
{
	struct Level_0
	{
		struct Wave{ uint32_t a_0 = 90;	string wave_path = "EXPL\EXP_NUCLEAR.wav"; };
	};
};
