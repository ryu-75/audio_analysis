#include "AudioAn.hpp"

/*
	ETAPES :
		- Rentrer dans le dossier via le chemin donné en argument
		- On vérifie le chemin de chaque fichier.
			Si l'extension est différent de .wav ou .mp3
				on l'ignore
			Sinon
				On stocke le chemin dans un container <list>
		- On sépare ensuite les fichiers dans un container correspondant
		à leur extension pour obtimiser l'analyse
		- On supprime ensuite le contenu du 1er container pour libérer
		de l'espace en mémoire
		- On analyse en suite chaque fichier audio à partir de leurs chemin :
			- On stock la taille de l'échantillon audio
*/

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Error: need a path" << std::endl;
		return (1);
	}
	else
	{
		try
		{
			std::string	path = av[1];
			AudioAn	audio(path);
		}
		catch(AudioAn::ParsException& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	return (0);
}
