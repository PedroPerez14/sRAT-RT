#include <string>
#include <iostream>
#include <sRAT-RT/settings.h>

struct Arguments
{
	std::string settings_file = "./settings/example_settings.ini";
	// TODO add more here as I think of them
};

void show_usage()
{
	std::cout << "Incorrect parameters! The correct usage is 'sRAT-RT [OPTIONS]'" << std::endl;
	std::cout << "Available OPTIONS: " << std::endl;
	std::cout << "\t-s, --settings: \t Specify the path to the .ini file containing the settings for the rendering, if unspecified default settings will be used." << std::endl;
	// TODO: Add more as I make progress
}

int main(int argc, char* argv[])
{
	Arguments args;
	if(argc == 1)
	{
		std::cout << "Using default settings file " << args.settings_file << std::endl;
	}
	else if(argc == 3)
	{
		args.settings_file = argv[2];
	}
	else
	{
		show_usage();
		return 1;
	}

	//TODO: Add more parameters for the constructor of the settings
	App app* = new App(args.settings_file);
	app.Run();
	
	return 0;
}