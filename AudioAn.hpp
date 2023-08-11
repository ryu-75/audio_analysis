#pragma once

#include <iostream>
#include <cstring>
#include <algorithm>
#include <list>

// template <typename T>
class AudioAn
{
	// ***************** ALIASES ********************
	public:
		typedef std::list<std::string>	files;

		AudioAn(char **av, uint _nbFiles);
		AudioAn(const AudioAn &lhs);

		AudioAn&	operator=(const AudioAn &copy);

		~AudioAn();

		bool	parsFilesExt(char *files);
		void	displayContent(void) const;

		class ParsException
		{
			public:
				ParsException(const std::string message) : _message(message) {};
			const char*	what() throw();
			private:
				std::string	_message;
		};
	private:
		AudioAn();
		uint	_nbFiles;
		files	_files;
};
