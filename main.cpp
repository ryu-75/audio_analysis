#include "AudioAn.hpp"

int	main(int ac, char **av)
{
	if (ac < 2)
	{
		std::cout << "Error: need at least one argument" << std::endl;
		return (1);
	}
	else
	{
		try
		{
			AudioAn	audio(av, (uint)ac);
		}
		catch(AudioAn::ParsException& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	return (0);
}
