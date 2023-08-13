#include "AudioAn.hpp"

const char*	AudioAn::ParsException::what() throw() { return (_message.c_str()); }

AudioAn::AudioAn(std::string path)
{
	parseFiles(path);
	fillContainer(this->_files);
	applyFFT(this->_wav);
	// displayContent(this->_wav);
}

AudioAn::AudioAn(const AudioAn &lhs)
{
	this->_files = lhs._files;
	*this = lhs;
}

AudioAn&	AudioAn::operator=(const AudioAn &copy)
{
	if (this != &copy)
	{
		this->_files = copy._files;
		return (*this);
	}
	return (*this);
}

AudioAn::~AudioAn() {}

bool	AudioAn::parsFilesExt(std::string files)
{
	for (size_t i = 0; i < files.size(); i++)
	{
		if (std::string::npos != files.find(".pdf") || \
			std::string::npos != files.find(".wav.asd"))
			return (false);
		else if (std::string::npos != files.find(".wav"))
			return (true);

	}
	return (false);
}

// template <typename T>
// void	AudioAn::displayContent(T& content) const
// {
// 	typename T::const_iterator	wavIt = content.begin();

// 	std::cout << "#\e[1m\e[34m[WAV]#\e[0m" << std::endl;
// 	for (; wavIt != content.end(); wavIt++)
// 	{
// 		std::cout << "\e[1m\e[34mFile\e[0m : " << wavIt->first << std::endl;
// 		std::cout << "\e[1m\e[35mSize\e[0m : " << wavIt->second << "Khz" << std::endl;
// 		if (std::next(wavIt) != content.end())
// 			std::cout << std::endl;
// 	}
// }

void	AudioAn::parseFiles(std::string path)
{
	DIR*	dir = opendir(path.c_str());
	std::string	filePath;

	if (dir == NULL)
		throw ParsException("Error: Cannot open the repository");
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG)
		{
			std::string	fileName = entry->d_name;

			filePath = path + "/" + fileName;
			if ((!fileName.empty() && fileName[0] == '.') ||\
				parsFilesExt(filePath) == false)
				continue ;
			else
				this->_files.push_back(filePath);
		}
	}
	if (this->_files.empty())
	{
		closedir(dir);
		throw ParsException("Error: is empty");
	}
	closedir(dir);
}

template <typename T>
void	AudioAn::fillContainer(T& content)
{
	typename T::const_iterator	it;

	for (it = content.begin(); it != content.end(); it++)
	{
		SF_INFO	fileInfo;
		SNDFILE*	sndfile = sf_open(it->c_str(), SFM_READ, &fileInfo);
		if (!sndfile)
		{
			std::cerr << sf_error(sndfile) << std::endl;
			throw ParsException("Error: cannot open the audio file");
		}
		int	size = fileInfo.samplerate;
		if (std::string::npos != it->find(".wav") && size != 0)
			this->_wav.push_back(std::make_pair(*it, size));
		sf_close(sndfile);
	}
	freeList(content);
}

template <typename T>
void	AudioAn::freeList(T& content)
{
	typename T::iterator	it = content.begin();

	while (it != content.end())
	{
		it->clear();
		it = content.erase(it);
	}
	content.clear();
}

template <typename T>
void	AudioAn::applyFFT(T& content)
{
	typename T::const_iterator	it = content.begin();
	for (; it != content.end(); it++)
	{
		SF_INFO	fileInfo;
		SNDFILE*	sndfile = sf_open(it->first.c_str(), SFM_READ, &fileInfo);
		if (!sndfile)
		{
			std::cerr << sf_error(sndfile) << std::endl;
			throw ParsException("Error: cannot open the audio file");
		}

		int	size = fileInfo.samplerate;

		std::vector<double> input(size);

		sf_read_double(sndfile, &input[0], size);

		applyHammingWindow(input);

		std::vector<fftw_complex> output(size / 2 + 1);
		fftw_plan	plan = fftw_plan_dft_r2c_1d(size, &input[0], &output[0], FFTW_ESTIMATE);
		if (!plan)
			throw ParsException("Error: creation FFT plan");

		fftw_execute(plan);

		highPassFilter(input, 31, size);
		lowPassFilter(output, 20000, size);

		int	peakIndex = findPeakIndex(output);

		double	sampleRate = static_cast<double>(size);
		double	fundamentalFreq = peakIndex * (sampleRate / size);

		displayContent(it->first, it->second, fundamentalFreq);
		
		convertFreq(fundamentalFreq);

		fftw_destroy_plan(plan);
		sf_close(sndfile);
	}
	fftw_cleanup();
}

void	AudioAn::displayContent(std::string filename, int sampleRate, double fundamentalFreq) const
{
	std::cout << "\e[1m\e[34mFilename :\e[0m" << filename << std::endl;
	std::cout << "\e[1m\e[35mSize :\e[0m " << sampleRate << "Hz" << std::endl;
	std::cout << "\e[1m\e[31mFundamental :\e[0m " << fundamentalFreq << "Hz" << std::endl;
}

int AudioAn::findPeakIndex(std::vector<fftw_complex>& output)
{
	int peakIndex = 0;
	double maxAmplitude = 0.0;
	int size = static_cast<int>(output.size());

	for (int i = 0; i < size; i++)
	{
		double amplitude = sqrt(output[i][0] * output[i][0] + output[i][1] * output[i][1]);
		if (amplitude > maxAmplitude)
		{
			maxAmplitude = amplitude;
			peakIndex = i;
		}
	}
	return peakIndex;
}

void	AudioAn::applyHammingWindow(std::vector<double>& input)
{
	int size = static_cast<int>(input.size());
	for (int i = 0; i < size; i++)
		input[i] *= (0.54 - 0.46 * cos(2 * M_PI * i / (size - 1)));
}

void	AudioAn::convertFreq(double fundamental)
{
	double	freqRef = 440.0;

	double	n = 12.0 * std::log2(fundamental / freqRef);
	while (n < 0)
		n += 12.0;

	const char*	key[] = { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };

	int	noteIndex = static_cast<int>(n + 0.5) % 12;
	double	semitonDiff = 12.0 * std::log2(fundamental / freqRef);
	int	octave = 4 + static_cast<int>(semitonDiff / 12.0);

	std::cout << "\e[1m\e[32mKey :\e[0m " << key[noteIndex] << octave << std::endl;
	std::cout << "\e[1mOctave :\e[0m " << octave << std::endl;
	std::cout << std::endl;
}

void	AudioAn::lowPassFilter(std::vector<fftw_complex>& output, double cutoffFrequency, double sampleRate)
{
	int	size = static_cast<int>(output.size());

	for (int i = 0; i < size; i++)
	{
		double	frequency = static_cast<double>(i) / size * sampleRate;
		if (frequency > cutoffFrequency)
		{
			output[i][0] = 0.0;
			output[i][1] = 0.0;
		}
	}
}

void	AudioAn::highPassFilter(std::vector<double>& input, double cutoffFrequency, double sampleRate)
{
	int	size = static_cast<int>(input.size());

	for (int i = 0; i < size; i++)
	{
		double	frequency = static_cast<double>(i) * sampleRate / size;
		if (frequency < cutoffFrequency)
			input[i] = 0.0;
	}
}
