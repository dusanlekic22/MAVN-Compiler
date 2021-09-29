/* Autor: Dusan Lekic Datum: 14.06.2020. */
#ifndef __LEXICAL_ANALYSIS__
#define __LEXICAL_ANALYSIS__

#include <fstream>
#include <vector>
#include <list>

#include "Token.h"
#include "FiniteStateMachine.h"


typedef std::list<Token> TokenList;


class LexicalAnalysis
{
public:
	/**
	 * Method for initializing the lexical analysis and FSM
	 */
	void initialize();

	/**
	 * Method which performs lexical analysis
	 */
	bool Do();

	/**
	 * Method for reading the input file
	 */
	bool readInputFile(std::string fileName);

	/**
	 * Use this function to get next lexical token from program source code.
	 *
	 * @return next lexical token in program source code
	 */
	Token getNextTokenLex();

	/**
	 * Use this function to get the list of tokens read from the source code
	 *
	 * @return list of tokens
	 */
	TokenList& getTokenList();

	/**
	 * Prints the token list
	 *
	 */
	void printTokens();

	/**
	 * Prints the errornous token if present
	 *
	 */
	void printLexError();

private:
	/**
	 * Input file containing program text to be analyzed
	 */
	std::ifstream m_inputFile;

	/**
	 * Program buffer containing the contents of the input files
	 */
	std::vector<char> m_programBuffer;

	/**
	 * Current position of the program buffer
	 */
	unsigned int m_programBufferPosition;

	/**
	 * Finite state machine object
	 */
	FiniteStateMachine m_fsm;

	/**
	 * List of parsed tokens
	 */
	TokenList m_tokenList;

	/**
	 * IF an error occurs while parsing this attribute will hold the errornous Token
	 */
	Token m_errorToken;

	/**
	 * Used for printing the test list. It decorates the output with header naming the columns
	 */
	void printMessageHeader();
};

#endif
