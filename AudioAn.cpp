#include "AudioAn.hpp"

const char*	AudioAn::ParsException::what() throw() { return (_message.c_str()); }

AudioAn::AudioAn(std::string path)
{
	parseFiles(path);
	fillContainer(this->_files);
	applyFFT(this->_wav);
	renameAll(this->_keys, this->_error, path);
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
			std::string::npos != files.find(".wav.asd") || \
				(!files.empty() && files[0] == '.'))
			return (false);
		else if (std::string::npos != files.find(".wav"))
			return (true);
	}
	return (false);
}

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
			filePath = path + fileName;
			if (parsFilesExt(filePath) == false)
			{
				this->_error.push_back(filePath);
				continue ;
			}
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

	this->_keys.push_back(std::make_pair(key[noteIndex], octave));

	std::cout << "\e[1m\e[32mKey :\e[0m " << key[noteIndex] << std::endl;
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

template <typename T, typename U>
void	AudioAn::renameAll(T& content, U& error, std::string path)
{
	DIR*	dir = opendir(path.c_str());

	if (dir == NULL)
		throw ParsException("Error: Cannot open the repository");

	struct dirent *entry;
	typename T::iterator	it = content.begin();
	int	i = 1;

	while ((entry = readdir(dir)) != NULL && it != content.end())
	{
		if (entry->d_type == DT_REG)
		{
			std::string	name = path  + entry->d_name;

			if (alreadyExist(name) == true)
			{
				SF_INFO	sfInfo;
				SNDFILE*	sndfile = sf_open(name.c_str(), SFM_READ, &sfInfo);
				if (parsFilesExt(name) == false)
					it++;
				else if (!sndfile)
				{
					std::cerr << "Error: unrecognised audio format" << std::endl;
					continue ;
				}
				else
				{
					std::cout << "old name : " << name << std::endl;
					std::string	index = std::to_string(i);
					std::string	newName;
					std::string octave = std::to_string(it->second);

					// REVOIR CETTE LIGNE
					if (checkSymbol(name) == false)
					{
						if (i < 10)
							newName = name.substr(0, name.find_last_of('_')) + '_' + it->first + octave + '_' + '0' + index + ".wav";
						else
							newName = name.substr(0, name.find_last_of('_')) + '_' + it->first + octave + '_' + index + ".wav";
					}
					else
					{
						if (i < 10)
							newName = name.substr(0, name.find_last_of('.')) + '_' + it->first + octave + '_' + '0' + index + ".wav";
						else
							newName = name.substr(0, name.find_last_of('.')) + '_' + it->first + octave + '_' + index + ".wav";
					}
					std::cout << "New name : " << newName << std::endl;
					if (rename(name.c_str(), newName.c_str()) != 0)
						throw ParsException("Error: Can not rename the audio file");
					i++;
				}
			}
			else
			{
				std::cout << name << ": key already exist in filename"<< std::endl;
				continue ;
			}
		}
		it++;
	}
	typename U::iterator	itEr = error.begin();
	for (; itEr != error.end(); itEr++)
		std::cerr << *itEr << ": accept wav format only"<< std::endl;
	closedir(dir);
}

bool	AudioAn::checkSymbol(std::string fileName)
{
	for (size_t i = 0; i < fileName.length(); i++)
	{
		if (fileName[i])
		{
			while (fileName[i] != '\0')
			{
				if (fileName[i] == '_')
					return (false);
				else if (fileName[i] == '.')
					return (true);
				i++;
			}
		}
	}
	return (false);
}

bool	AudioAn::alreadyExist(std::string filename)
{
	const char*	keys[12] = { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };

	for (size_t i = 0; i < 12; i++)
	{
		for (size_t y = 0; keys[i][y] != '\0'; y++)
		{
			for (size_t j = 0; j < filename.size(); j++)
				if (filename[j] == '_' && filename[j + 1] == keys[i][y])
					return (false);
		}
	}
	return (true);
}
