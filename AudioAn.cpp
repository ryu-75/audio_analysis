#include "AudioAn.hpp"

const char*	AudioAn::ParsException::what() throw() { return (_message.c_str()); }

AudioAn::AudioAn(string path)
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

bool	AudioAn::parsFilesExt(string files)
{
	for (string::size_type i = 0; i < files.size(); i++)
	{
		bool	hasPdf = files.find(".pdf") != string::npos;
		bool	hasWavAsd = files.find(".wav.asd") != string::npos;
		bool	startWithDot = !files.empty() && files[0] == '.';
		if (hasPdf || hasWavAsd || startWithDot)
			return (false);
		else if (string::npos != files.find(".wav"))
			return (true);
	}
	return (false);
}

void	AudioAn::parseFiles(string path)
{
	DIR*	dir = opendir(path.c_str());
	string	filePath;

	if (dir == nullptr)
		throw ParsException("Error: Cannot open the repository");
	struct dirent *entry;
	while ((entry = readdir(dir)) != nullptr)
	{
		if (entry->d_type == DT_REG)
		{
			string	fileName = entry->d_name;
			filePath = path + '/' + fileName;
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
void	AudioAn::print(T& files)
{
	typename T::iterator	it = files.begin();
	while (it != files.end()) 
	{
		cout << "Files name: " << *it << endl;
		it++;
	}
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

		if (string::npos != it->find(".wav") && size != 0)
			this->_wav.push_back(make_pair(*it, size));
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
		vector<double> input(size);

		sf_read_double(sndfile, &input[0], size);
		applyHammingWindow(input);

		vector<fftw_complex> output(size / 2 + 1);
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

void	AudioAn::displayContent(string filename, int sampleRate, double fundamentalFreq) const
{
	cout << "\e[1m\e[34mFilename :\e[0m" << filename << endl;
	cout << "\e[1m\e[35mSize :\e[0m " << sampleRate << "Hz" << endl;
	cout << "\e[1m\e[31mFundamental :\e[0m " << fundamentalFreq << "Hz" << endl;
}

int AudioAn::findPeakIndex(vector<fftw_complex>& output)
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

void	AudioAn::applyHammingWindow(vector<double>& input)
{
	int size = static_cast<int>(input.size());
	for (int i = 0; i < size; i++)
		input[i] *= (0.54 - 0.46 * cos(2 * M_PI * i / (size - 1)));
}

void	AudioAn::convertFreq(double fundamental)
{
	double	freqRef = 440.0;

	double	n = 12.0 * log2(fundamental / freqRef);
	while (n < 0)
		n += 12.0;

	const char*	key[] = { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };

	int	noteIndex = static_cast<int>(n + 0.5) % 12;
	this->_keys.push_back(key[noteIndex]);

	cout << "\e[1m\e[32mKey :\e[0m " << key[noteIndex] << endl;
	cout << endl;
}

void	AudioAn::lowPassFilter(vector<fftw_complex>& output, double cutoffFrequency, double sampleRate)
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

void	AudioAn::highPassFilter(vector<double>& input, double cutoffFrequency, double sampleRate)
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
void	AudioAn::renameAll(T& notes, U& error, string path)
{
	DIR*	dir = opendir(path.c_str());

	if (dir == nullptr) throw ParsException("Error: Cannot open the repository");

	struct dirent *entry;
	typename T::iterator	it = notes.begin();
	int	i = 1;

	vector<string>	file;

	while ((entry = readdir(dir)) != nullptr && it != notes.end())
	{
		if (entry->d_type == DT_DIR || entry->d_type == DT_REG)
		{
			if (string(entry->d_name) == "." || string(entry->d_name) == "..")
			{
				continue ;
			}
			else 
			{
				string	name = path + '/' + entry->d_name;
				string	extName = "." + getAudioExtension(name);
				SF_INFO	sfInfo;
				SNDFILE*	sndfile = sf_open(name.c_str(), SFM_READ, &sfInfo);
				if (parsFilesExt(name) == false)
					continue ;
				else if (!sndfile)
				{
					cerr << "Error: unrecognised audio format" << endl;
					continue ;
				}
				else
				{
					cout << "old name : " << name << endl;
					string	newName;
					string	index = to_string(i);
					if (checkSymbol(name) == false)
					{
						string	dirName = name.substr(0, name.find_first_of("/"));
						string	fileName = name.substr(dirName.length() + 1);
						if (alreadyExist(fileName)) 
							newName = dirName + '/' + fileName.substr(0, fileName.find_last_of(".")) + '_' + *it + '_' + (i++ < 10 ? '0' + index : index) + extName;
						else
							cerr << "Is already renamed" << endl;
						cout << "New name: " << newName << endl;
					}
					if (rename(name.c_str(), newName.c_str()) != 0)
						throw ParsException("Error: Cannot rename the audio file");
				}
			}
		}
		else 
			cerr << "It's not a directory or a regular file" << endl;
		it++;
	}
	typename U::iterator	itEr = error.begin();
	for (; itEr != error.end(); itEr++)
		cerr << *itEr << ": accept wav format only"<< endl;
	closedir(dir);
}

string	AudioAn::getAudioExtension(const string &filename) {
	std::size_t	dotPos = filename.rfind(".");

	if (dotPos != std::string::npos) {
		string	ext = filename.substr(dotPos + 1);
		if (ext == "wav")
			return ext;
	}
	return "";
}

bool	AudioAn::checkSymbol(string fileName)
{
	if (!fileName.empty())
	{
		for (string::size_type i = 0; i < fileName.length(); i++)
		{
			if (fileName[i])
			{
				while (fileName[i] != '\0')
				{
					if (fileName[i] == '.')				
						return (true);
					else if (fileName[i] == '_')
						return (false);
					i++;
				}
			}
		}
	} else {
		throw ParsException("Filename is empty");
	}
	return (false);
}

bool	AudioAn::alreadyExist(string filename)
{
	const char*	keys[] = { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };

	for (const char* key : keys) 
	{
		string	keyString = key;
		std::size_t	pos = filename.find(keyString);
		while (pos != std::string::npos)
		{
			if (filename[pos + keyString.length()] != '_') return (true);
			pos = filename.find(keyString, pos + 1);
		}
	}
	return (false);
}
