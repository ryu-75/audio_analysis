#pragma once

#include <iostream>
#include <cstring>
#include <algorithm>
#include <list>
#include <dirent.h>
#include <sys/types.h>
#include <filesystem>

class AudioAn
{
	public:
		// ***************** ALIASES ********************
		typedef std::list<std::string>	files;
		typedef std::list<std::string>	wav;
		typedef std::list<std::string>	mp3;

		// ***************** STRUCTURE ******************
		struct dirent	*entry;

		AudioAn(std::string path);
		AudioAn(const AudioAn &lhs);

		template <typename T>
		void	freeList(T& content);

		template <typename T>
		void	splitByExt(T& content);

		AudioAn&	operator=(const AudioAn &copy);

		~AudioAn();

		bool	parsFilesExt(std::string files);
		void	displayContent(void) const;
		void	parseFiles(std::string path);

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
		files	_files;
		wav	_wav;
		mp3	_mp3;
};
