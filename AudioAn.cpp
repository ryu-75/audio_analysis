#include "AudioAn.hpp"

const char*	AudioAn::ParsException::what() throw() { return (_message.c_str()); }

AudioAn::AudioAn(char  **av, uint nbFiles)
{
	this->_nbFiles = nbFiles;
	for (size_t i = 1; i < this->_nbFiles; i++)
	{
		if (parsFilesExt(av[i]) == 1)
			throw ParsException("Error: bad extension");
		this->_files.push_back(av[i]);
	}
	displayContent();
}

AudioAn::AudioAn(const AudioAn &lhs)
{
	this->_nbFiles = lhs._nbFiles;
	this->_files = lhs._files;
	*this = lhs;
}

AudioAn&	AudioAn::operator=(const AudioAn &copy)
{
	if (this != &copy)
	{
		this->_nbFiles = copy._nbFiles;
		this->_files = copy._files;
		return (*this);
	}
	return (*this);
}

AudioAn::~AudioAn() {}

bool	AudioAn::parsFilesExt(char *files)
{
	std::string	f = files;

	for (size_t i = 0; i < f.size(); i++)
	{
		if (std::string::npos != f.find(".wav") ||\
			std::string::npos != f.find(".mp3"))
				return (0);
	}
	return (1);
}

void	AudioAn::displayContent(void) const
{
	std::list<std::string>::const_iterator	it = this->_files.begin();
	for (; it != this->_files.end(); it++)
		std::cout << *it << std::endl;
}
