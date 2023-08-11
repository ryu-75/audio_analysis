#include "AudioAn.hpp"

const char*	AudioAn::ParsException::what() throw() { return (_message.c_str()); }

AudioAn::AudioAn(std::string path)
{
	parseFiles(path);
	splitByExt(this->_files);
	displayContent();
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
		if (std::string::npos != files.find(".wav") ||\
			std::string::npos != files.find(".mp3"))
				return (true);
	}
	return (false);
}

void	AudioAn::displayContent(void) const
{
	std::cout << "#\e[1m\e[34m[MP3]#\e[0m" << std::endl;
	std::list<std::string>::const_iterator	mp3 = this->_mp3.begin();
	for (; mp3 != this->_mp3.end(); mp3++)
		std::cout << *mp3 << std::endl;

	std::cout << "#\e[1m\e[34m[WAV]#\e[0m" << std::endl;
	std::list<std::string>::const_iterator	wav = this->_wav.begin();
	for (; wav != this->_wav.end(); wav++)
		std::cout << *wav << std::endl;
}

void	AudioAn::parseFiles(std::string path)
{
	DIR*	dir = opendir(path.c_str());
	std::string	filePath;

	if (dir == NULL)
		throw ParsException("Error: Cannot open the repository");
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG)
		{
			filePath = path + "/" + entry->d_name;
			if (parsFilesExt(filePath) == false)
				continue ;
			else
				this->_files.push_back(filePath);
		}
	}
	if (this->_files.empty())
		throw ParsException("Error: is empty");
	closedir(dir);
}

template <typename T>
void	AudioAn::splitByExt(T& content)
{
	typename T::const_iterator	it = content.begin();

	for (; it != content.end(); it++)
	{
		if (std::string::npos != it->find(".wav"))
			this->_wav.push_back(*it);
		else if (std::string::npos != it->find(".mp3"))
			this->_mp3.push_back(*it);
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
