What is this software package for?
***********************************
The programs in this package are used to decompile and compile .wpk files. WpkExporter is the successor to WPKtoTXT (written by Atlas), which exported .wpk files to .txt format. 
WpkExporter differs in that it supports other versions of .wpk from other Reality Pump games. The program also allows export to .cpp format.

WpkImporter, on the other hand, is the successor to Wpconvert.exe, created by Reality Pump and included with the Earth 2160 SDK. 
WpkImporter was written to mimic the behavior of Wpconvert, but adds a new function for compiling cpp files.

-> WpkImporter and WpkExporter were written entirely by Atlas.

WpkExporter
***********

This program is used to convert .wpk files, i.e., wavepack files, to .txt or .cpp files.
Start the program and enter the name of the .wpk file, then the program will ask you what the output format should be.
Select txt or cpp by entering one of these formats and press Enter. The program will create a ready cpp or txt file.

The program supports .wpk files from the following games:
-> Earth 2150: Escape from the Blue Planet,
-> Earth 2150: The Moon Project,
-> Earth 2150: Lost Souls,
-> Heli Heroes,
-> World War II: Panzer Claws/Frontline Attack: War Over Europe,
-> World War III: Black Gold,
-> KnightShift/Polanie 2/Once Upon a Knight,
-> KnightShift II Curse of Souls/Polanie 3,
-> Earth 2160,

The program also works in ARGC&ARGV mode:

cmd:
WpkExporter.exe <path_to_wpk_file> <cpp/txt>

powershell:
.\WpkExporter.exe <path_to_wpk_file> <cpp/txt>

Example:
WpkExporter.exe D:\directory\file.wpk cpp

-> We can immediately decompile all .wpk files in the directory to .txt using the file: _EXPORT_ALL_TO_TXT.bat or _EXPORT_ALL_TO_TXT.ps1
-> We can also immediately decompile all .wpk files in the directory to .cpp using the file: _EXPORT_ALL_TO_CPP.bat or _EXPORT_ALL_TO_CPP.ps1

----------------------------------------------------------------------------------------------------------------------------

Editing .txt or .cpp files
***************************
Depending on the game from which we have the .wpk file, we will get a slightly different output structure of the cpp or txt file.

Txt editing:
***********

Example txt for KnightShift:

wavepack TALK_AMAZONE_COMMAND
{
	Level0 9
	{
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_1.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_2.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_3.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_4.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_5.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_6.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_7.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_8.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_9.wav
	}
	Level1 9
	{
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_1.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_2.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_3.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_4.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_5.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_6.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_7.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_8.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_9.wav
	}
}

Example txt for ww3 black gold and heli heroes:

wavepack RUS_M07
{
	Type 0 <------------------------ If we want to keep this wpk format for this game, 
	DisableLevel2 1 <--------------- we do not change these two values.
	Level0 1
	{
		Wave 100 MESSAGE\RUS\RUS_M07.WAV
        ... <--- Here we add Wave entries.
	}
    Level1 ... <---- We can create an additional level of Waves.
    {
        ...
    }
}

-> The Type instruction specifies the type of wavepack.
-> DisableLevel2 is an instruction used to block Level2. In other words, it is an instruction that allows you to create a .wpk format for the game Earth 2150.
-> If we want to keep the format from a given game, we do not touch these two instructions.
-> Wavepacks are best edited using another wavepack as an example.
-> Multiple wavepacks can be defined in a single txt file, as shown in the Sample.txt file in the WpkImporter directory. 
Defining multiple wavepacks in a single txt file creates multiple wpk files and is faster than running each txt file separately and compiling it.
-> The downside of a txt file is that you have to stick closely to the syntax.
-> If tab indentation is used, we also use tab indentation.
-> We strive to create files based on templates and maintain aesthetics.
-> The number of waves in a given Level should match the number given next to the Level instructions.
-> The program has a built-in debugger that flags errors that need to be corrected. However, this debugger cannot predict everything, so just don't make mistakes.
-> It is best to use a text editor that allows you to see all characters. An example of such a program is Notepad++, which has a Show All Characters option.

Editing cpp
***********

Example of a cpp file:

uint8_t format_0 = 87; // W
uint8_t format_1 = 80; // P
uint8_t format_2 = 75; // K
uint8_t format_3 = 0;

uint32_t wavepack_type = 0;
uint32_t number_of_waves_in_level_0 = count();
uint32_t number_of_waves_in_level_1 = count();

uint32_t data_size = measure_all_value_sizes_after_this_instruction();

namespace ww3_black_gold_test
{
	struct Level_0
	{
		struct Wave{ uint32_t a_0 = 100;	string wave_path = "MESSAGE\RUS\RUS_M07.WAV"; };
	};
};

-> The cpp file is characterized by the fact that we can edit literally every value in the file.
-> A cpp file can only store one wavepack.
-> If we want to edit a given wavepack from a given game and preserve its format so that it works in that game, 
we only edit the Level struct and copy the line struct Wave{...} and paste it below.
-> All data above the namespace does not need to be edited.
-> 32bituniquekey c_1 is a value type uint32_t, which can take the constants: NULL, i.e. 0, or DELAYED, i.e. 1.

Example of a correctly edited cpp file:

uint8_t format_0 = 87; // W
uint8_t format_1 = 80; // P
uint8_t format_2 = 75; // K
uint8_t format_3 = 0;

uint32_t wavepack_type = 0;
uint32_t number_of_waves_in_level_0 = count();
uint32_t number_of_waves_in_level_1 = count();

uint32_t data_size = measure_all_value_sizes_after_this_instruction();

namespace ww3_black_gold_test
{
	struct Level_0
	{
		struct Wave{ uint32_t a_0 = 100;	string wave_path = "MESSAGE\RUS\RUS_M07.WAV"; };
        struct Wave{ uint32_t a_0 = 95;	    string wave_path = "MESSAGE\RUS\RUS_M08.WAV"; };
        struct Wave{ uint32_t a_0 = 10;	    string wave_path = "MESSAGE\RUS\RUS_M09.WAV"; };
	};

    struct Level_1
    {
        struct Wave{ uint32_t a_0 = 100;	string wave_path = "MESSAGE\RUS\test_M01.WAV"; };
        struct Wave{ uint32_t a_0 = 2;	    string wave_path = "MESSAGE\RUS\test_M02.WAV"; };
        struct Wave{ uint32_t a_0 = 34;	    string wave_path = "MESSAGE\RUS\test_M03.WAV"; };
        struct Wave{ uint32_t a_0 = 55;	    string wave_path = "MESSAGE\RUS\test_M04.WAV"; };
        struct Wave{ uint32_t a_0 = 100;	string wave_path = "MESSAGE\RUS\test_M05.WAV"; };
    };

};

-> As we can see, the struct Level does not specify the number of waves, because WpkImporter will count the number of waves during compilation and enter it in the number_of_waves_in_level_<index> field. 
The program detects the count() and measure_all_value_sizes_after_this_instruction() functions, so we don't have to worry about counting waves.

----------------------------------------------------------------------------------------------------------------------------

WpkImporter
***********

This program is used to convert .txt and .cpp files to .wpk, i.e., wavepack.
After launching the program, it will ask us for the name of the input file. We enter the name of the file in txt or cpp format and press Enter.
The program will create the finished .wpk file or files.

Editing the txt structure is simple. Just look through the Sample.txt file.
Next to the label level0 there is a number, which indicates the number of wave entries at that level.
So if I have 9 wave entries, I have to enter 9. If I have nothing at level1, I can delete it completely, but level0 must always be there.

One txt file can contain multiple wavepacks, while one cpp file can contain only one wavepack.

-> The program compiles all .txt files in the directory via the file _CONVERT_ALL_TXT.bat or _CONVERT_ALL_TXT.ps1.
-> The program compiles all .cpp files in the directory via the file _CONVERT_ALL_CPP.bat or _CONVERT_ALL_CPP.ps1.

The program also works in ARGC&ARGV mode:

cmd:
WpkImporter.exe <path_to_txt_or_cpp_file>

powershell:
.\WpkImporter.exe <path_to_txt_or_cpp_file>

Example:
WpkImporter.exe D:\directory\all_wpks.txt

----------------------------------------------------------------------------------------------------------------------------

The programs were tested on:
***************************
-> 566 .wpk files (i.e. all) from the game Earth 2150: Escape from the Blue Planet - test result 100%,
-> 289 .wpk files (i.e. all) from the game Earth 2150: The Moon Project - test result 100%,
-> 289 .wpk files (i.e. all) from the game Earth 2150: Lost Souls - test result 100%,
-> 70 .wpk files (i.e. all) from the game Heli Heroes - test result 100%,
-> 218 .wpk files (i.e. all) from the game World War II: Panzer Claws/Frontline Attack: War Over Europe - test result 100%,
-> 261 .wpk files (i.e. all) from the game World War III: Black Gold - test result 100%,
-> 424 .wpk files (i.e. all) from the game KnightShift/Polanie 2/Once Upon a Knight - test result 100%,
-> 1065 .wpk files (i.e. all) from the game KnightShift II Curse of Souls/Polanie 3 - test result 100%,
-> 961 .wpk files (i.e. all) from the game Earth 2160 - test result 100%,

The programs were created by Atlas.

