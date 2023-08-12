#pragma once

#include <iostream>
#include <cstring>
#include <algorithm>
#include <list>
#include <dirent.h>
#include <sys/types.h>
#include <filesystem>
#include <sndfile.hh>
#include <sndfile.h>
#include <fftw3.h>
#include <cmath>

class AudioAn
{
	public:
		// ***************** ALIASES ********************
		typedef std::list<std::string>	files;
		typedef std::list<std::pair<std::string, const int >>	wav;

		// ***************** STRUCTURE ******************

		// *************** CONST / DEST *****************
		AudioAn(std::string path);
		AudioAn(const AudioAn &lhs);
		AudioAn&	operator=(const AudioAn &copy);
		~AudioAn();

		// ************** TEMPLATE METHODS **************
		template <typename T>
		void	freeList(T& content);

		template <typename T>
		void	fillContainer(T& content);

		template <typename T>
		void	displayContent(T& cont1) const;

		template <typename T>
		void	applyFFT(T& content);

		// ************** METHODS ***********************
		bool	parsFilesExt(std::string files);
		void	parseFiles(std::string path);
		int	samplingSize(std::string path);
		int findPeakIndex(std::vector<fftw_complex>& output);
		void applyHammingWindow(std::vector<double>& input);

		// ************** EXCEPTION *********************
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
};
