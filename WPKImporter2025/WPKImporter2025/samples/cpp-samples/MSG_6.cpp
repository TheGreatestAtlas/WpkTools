uint8_t format_0 = 87; // W
uint8_t format_1 = 80; // P
uint8_t format_2 = 75; // K
uint8_t format_3 = 0;

uint32_t wavepack_type = 1;
uint32_t number_of_waves_in_level_0 = count();
uint32_t number_of_waves_in_level_1 = count();

uint32_t data_size = measure_all_value_sizes_after_this_instruction();

namespace MSG_6
{
	struct Level_0
	{
		struct Wave{ uint32_t a_0 = 100;	uint32_t a_1 = 30;	uint32_t a_2 = 35;	uint8_t b_0 = 255;	uint8_t b_1 = 255;	uint32_t c_0 = 0;	32bituniquekey c_1 = DELAYED;	uint32_t c_2 = 0;	string wave_path = "MESSAGES\Msg_6.WAV"; };
	};
};
