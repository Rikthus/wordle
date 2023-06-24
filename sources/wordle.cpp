#include "../includes/wordle.hpp"

static	void	printWelcome(void)
{
	string	welcomeStr[12] = {
		"#####################################################################################",
		"#                                                                                   #",
		"#         **       **    *******    *******    *******    **        ********        #",
		"#        /**      /**   **/////**  /**////**  /**////**  /**       /**/////         #",
		"#        /**   *  /**  **     //** /**   /**  /**    /** /**       /**              #",
		"#        /**  *** /** /**      /** /*******   /**    /** /**       /*******         #",
		"#        /** **/**/** /**      /** /**///**   /**    /** /**       /**////          #",
		"#        /**** //**** //**     **  /**  //**  /**    **  /**       /**              #",
		"#        /**/   ///**  //*******   /**   //** /*******   /******** /********        #",
		"#        //       //    ///////    //     //  ///////    ////////  ////////         #",
		"#                                                                                   #",
		"#####################################################################################"
	};
	for (int i = 0; i < 12; i++)
		std::cout << BLUE << welcomeStr[i] << END << endl;
}

static	void	printRules(unsigned int dictSize)
{
	cout << endl << endl;
	cout << "RULES OF THE GAME:" << endl << endl;
	cout << "1#   6 guesses to find the WORDLE" << endl;
	cout << "2#   you can only enter five letter WORDLES" << endl;
	cout << "3#   the letters can only be lowercase" << endl;
	cout << "4#   a green letter means it is valid and at the right place in the WORDLE" << endl;
	cout << "5#   a yellow letter means it is valid but not at the right place in the WORDLE" << endl;
	cout << "6#   a grey letter means it is not valid for the WORDLE" << endl;
	cout << endl << "7#   find the WORDLE" << endl;
	cout << endl << "Today the pool of WORDLE size is: " + to_string(dictSize) << endl;
	cout << endl << endl;
}

static	void	closeAndThrow(ifstream &inputStream, string msg)
{
	inputStream.close();
	throw(runtime_error(msg));
}

static	ifstream	openDictionnary(void)
{
	ifstream 	dict;
	struct stat fileStat;

	dict.open("dictionnary.txt", ifstream::in);
	if (!dict.is_open())
		throw (runtime_error("Error: Could not open dictionnary './dictionnary.txt'"));
	else if (!(stat("./dictionnary.txt", &fileStat) == 0 && S_ISREG(fileStat.st_mode)))
		throw (runtime_error("Error: './dictionnary.txt' is not a regular file"));
	return (dict);
}

static	unsigned	int	parseDictionnary(ifstream &dict)
{
	unsigned	int	wordCount = 0;
	string			word;

	while (!dict.eof() && getline(dict, word))
	{
		wordCount++;
		if (word.empty())
			closeAndThrow(dict, "Error: dictionnary line contains nothing");
		else if (wordCount > MAX_DICT_SIZE)
			closeAndThrow(dict, "Error: dictionnary has more than " + to_string(MAX_DICT_SIZE) + " entries");
		else if (word.length() != 5)
			closeAndThrow(dict, "Error: dictionnary entry " + to_string(wordCount) + " does not have a length of 5");
		for (unsigned int i = 0; i < word.size(); i++)
		{
			if (!islower(word[i]))
				closeAndThrow(dict, "Error: dictionnary entry " + to_string(wordCount) + " does not contain only lowercase letters");
		}
	}
	if (wordCount < MIN_DICT_SIZE)
		closeAndThrow(dict, "Error: dictionnary has less than " + to_string(MIN_DICT_SIZE) + " entries");
	if (!dict.eof() && !dict.good())
		closeAndThrow(dict, "Error: error while reading dictionnary file");
	dict.seekg(0, std::ios::beg);
	return (wordCount);
}

static	string	generateWord(unsigned	int dictSize, ifstream &dict)
{
	// unsigned	int const		randMin = 1;
	unsigned	int const		randMax = dictSize;
	unsigned	int				wordIndex;
	string						word;

	random_device			 	rd;
	mt19937						gen(rd());
	uniform_int_distribution<>	distrib(randMax, randMax);

	wordIndex = distrib(gen);
	for (unsigned int i = 0; i < wordIndex; i++)
	{
		getline(dict, word);
		if (dict.fail() || word.empty())
			closeAndThrow(dict, "Error: error while reading dictionnary file");
	}
	return (word);
}

static	void	printHelp(string test, string word)
{
	static	vector<char>	validLetters;
	static	vector<char>	invalidLetters;

	for (unsigned int i = 0; i < test.size(); i++)
	{
		if (count(word.begin(), word.end(), test[i]) > 0)
		{
			if (count(validLetters.begin(), validLetters.end(), test[i]) == 0)
				validLetters.push_back(test[i]);
		}
		else
		{
			if (count(invalidLetters.begin(), invalidLetters.end(), test[i]) == 0)
				invalidLetters.push_back(test[i]);
		}
	}

	cout << endl;
	cout << "How many letters in the WORDLE =>   ";
	for (unsigned int i = 0; i < test.size(); i++)
		cout << test[i] << ": " << count(word.begin(), word.end(), test[i]) << "  ";
	cout << endl;
	cout << "GOOD letters =>   ";
	for (unsigned int i = 0; i < validLetters.size(); i++)
		cout << validLetters[i] << "  ";
	cout << endl;
	cout << "BAD letters =>   ";
	for (unsigned int i = 0; i < invalidLetters.size(); i++)
		cout << invalidLetters[i] << "  ";
}

static	void	printLetters(string test, string word)
{
	for (unsigned int i = 0; i < test.size(); i++)
	{
		if (test[i] == word[i])
			cout << GREEN << test[i] << END;
		else if (count(word.begin(), word.end(), test[i]) == 0)
			cout << GREY << test[i] << END;
		else
			cout << YELLOW << test[i] << END;
	}
}

static	void	startGame(string wordToFind)
{
	int		nbGuess = 0;
	string	mode;
	string	test;

	cout << "Enter your preferred mode:   easy | normal" << endl;
	while (getline(cin, mode) && !cin.eof())
	{
		if (mode == "easy" || mode == "normal")
			break;
	}
	if (cin.eof())
		return;
	cin.clear();
	cout << endl << "The mode is now: " << mode << endl << BLUE << "AND THE GAME BEGINS!" << END << endl << endl;
	cout << "=>   ";
	while (nbGuess < 6 && getline(cin, test) && !cin.eof())
	{
		if (test.empty() || test.size() != 5)
		{
			cout << "Please enter a valid WORDLE" << endl;
			continue;
		}
		for (char c: test)
		{
			if (!islower(c))
			{
				cout << "Please enter a valid WORDLE" << endl;
				continue;
			}
		}

		nbGuess++;
		cout << endl;
		cout << "WORDLE:       ";
		printLetters(test, wordToFind);
		cout << endl;
		cout << "guesses: " + to_string(nbGuess) << endl;
		if (test == wordToFind)
			break;
		if (mode == "easy")
			printHelp(test, wordToFind);
		cout << endl;
		cout << endl;
		cout << endl;
		cout << "=>   ";
	}
	cout << endl << endl;
	if (nbGuess == 6 && test != wordToFind)
		cout << RED << "You have been defeated by the WORDLE:    " + wordToFind << END << endl;
	else if (test == wordToFind)
		cout << BLUE << "Congratulations you have found the WORDLE in " + to_string(nbGuess) + " try!" << END << endl;
}

int	main(int argc, char **argv)
{
	(void)argv;
	if (argc != 1)
	{
		std::cerr << "Usage: ./wordle" << std::endl;
		return (0);
	}
	try
	{
		ifstream		dict = openDictionnary();
		unsigned	int	dictSize = parseDictionnary(dict);
		string			wordOfTheDay = generateWord(dictSize, dict);

		dict.close();

		printWelcome();
		printRules(dictSize);
		startGame(wordOfTheDay);
	}
	catch	(std::exception &e)
	{
		cerr << e.what() << endl;
		return (1);
	}
	return (0);
}