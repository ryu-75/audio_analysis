#pragma once

#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <list>
#include <dirent.h>
#include <sys/types.h>
#include <filesystem>
#include <sndfile.hh>
#include <sndfile.h>
#include <fftw3.h>
#include <cmath>
#include <sstream>

// Namespace
using	std::string;
using	std::list; 
using	std::vector;
using	std::pair;
using	std::size_t;
using	std::make_pair;
using	std::cout;
using	std::endl;
using	std::log2;
using	std::cerr;
using	std::to_string;

class AudioAn
{
	public:
		// ***************** ALIASES ********************
		typedef list<string>	files;
		typedef list<string>	keys;
		typedef list<pair<string, const int >>	wav;
		typedef list<string>	error;
		// *************** CONST / DEST *****************
		AudioAn(string path);
		AudioAn(const AudioAn &lhs);
		AudioAn&	operator=(const AudioAn &copy);
		~AudioAn();

		// ************** TEMPLATE METHODS **************
		template <typename T>
		void	freeList(T& content);

		template <typename T>
		void	fillContainer(T& content);

		template <typename T, typename U>
		void	renameAll(T& notes, U& error, const string &path);

		template <typename T>
		void	applyFFT(T& content);

		template <typename T>
		void	print(T& files);

		// ************** METHODS ***********************
		void	parseFiles(string path);
		int		samplingSize(string path);
		int		findPeakIndex(vector<fftw_complex>& output);
		void	applyHammingWindow(vector<double>& input);
		void	convertFreq(double fundamental);
		void	lowPassFilter(vector<fftw_complex>& output, double cutoff, double sampleRate);
		void	highPassFilter(vector<double>& input, double cutoffFrequency, double sampleRate);
		void	displayContent(string filename, int sampleRate, double fundamentalFreq) const;
		bool	checkSymbol(string fileName);
		bool	alreadyExist(string fileName);
		bool	parsFilesExt(string files);
		bool	checkPath(const string &path);
		string	getAudioExtension(const string &filename);


		// ************** EXCEPTION *********************
		class ParsException
		{
			public:
				ParsException(const string message) : _message(message) {};
			const char*	what() throw();
			private:
				string	_message;
		};
	private:
		AudioAn();
		files	_files;
		wav	_wav;
		keys	_keys;
		error	_error;
};
