/* Autor: Dusan Lekic Datum: 14.06.2020. */
#include "LexicalAnalysis.h"

#include "Constants.h"
#include "Token.h"

#include <iostream>
#include <iomanip>
#include <fstream>


using namespace std;


void LexicalAnalysis::initialize()
{
	m_programBufferPosition = 0;
	m_fsm.initStateMachine();
}


bool LexicalAnalysis::Do()
{
	while (true)
	{
		Token token = getNextTokenLex();
		switch (token.getType())
		{
			case T_ERROR:
				m_errorToken = token;
				m_tokenList.push_back(token);
				return false;
			case T_END_OF_FILE:
				m_tokenList.push_back(token);
				return true;
			case T_WHITE_SPACE:
				continue;
			default:
				m_tokenList.push_back(token);
				break;
		}
	}
}


bool LexicalAnalysis::readInputFile(string fileName)
{
	m_inputFile.open(fileName, ios_base::binary);

	if (!m_inputFile)
		return false;
	
	m_inputFile.seekg(0, m_inputFile.end);
	int length = (int)m_inputFile.tellg();
	m_inputFile.seekg (0, m_inputFile.beg);
	m_programBuffer.resize(length);
	if (length > 0)
		m_inputFile.read(&m_programBuffer.front(), length);
	m_inputFile.close();
	return true;
}


Token LexicalAnalysis::getNextTokenLex()
{
	int currentState = START_STATE;
	int nextState = 0;
	int lastFiniteState = 0;

	// position in stream
	int counter = 0;
	int lastLetterPos = m_programBufferPosition;

	Token token;

	while (true)
	{
		char letter;
		unsigned int letterIndex = m_programBufferPosition + counter;

		if (letterIndex < m_programBuffer.size())
		{
			letter = m_programBuffer[letterIndex];
		}
		else
		{
			// we have reached the end of input file, force the search letter to invalid value
			letter = -1;
			if (m_programBufferPosition >= m_programBuffer.size())
			{
				// if we have reached end of file and printed out the last correct token
				// create EOF token and exit
				token.makeEofToken();
				return token;
			}
		}
		
		nextState = this->m_fsm.getNextState(currentState, letter);
		counter ++;

		if (nextState > IDLE_STATE)
		{
			// change the current state
			currentState = nextState;

			if (nextState == START_STATE)
				throw runtime_error("\nException: Infinite state detected! There is something very wrong with the code !\n");

			// remember last finite state
			lastFiniteState = nextState;
			lastLetterPos = m_programBufferPosition + counter;
		}
		else if (nextState == INVALID_STATE)
		{
			// eof character read, generate token defined with last finite state
			if (lastFiniteState != IDLE_STATE)
			{
				// token recognized, make token
				token.makeToken(m_programBufferPosition, lastLetterPos, m_programBuffer, lastFiniteState);
				m_programBufferPosition = lastLetterPos;
				return token;
			}
			else
			{
				// error occurred, create error token
				token.makeErrorToken(m_programBufferPosition + counter - 1, m_programBuffer);
				m_programBufferPosition = m_programBufferPosition + counter - 1;
				return token;
			}
		}
		else
		{
			// final state reached, state machine is in IDLE state
			// calculate the number of characters needed for the recognized token
			int len = lastLetterPos - m_programBufferPosition;

			// create the token
			if (len > 0)
			{
				// token recognized, make token
				token.makeToken(m_programBufferPosition, lastLetterPos, m_programBuffer, lastFiniteState);
				m_programBufferPosition = lastLetterPos;
				return token;
			}
			else
			{
				// error occurred, create error token
				token.makeErrorToken(m_programBufferPosition + counter - 1, m_programBuffer);
				m_programBufferPosition = m_programBufferPosition + counter - 1;
				return token;
			}
		}
	}

	return token;
}


TokenList & LexicalAnalysis::getTokenList()
{
	return m_tokenList;
}

void LexicalAnalysis::printTokens()
{
	if (m_tokenList.empty())
	{
		cout << "Token list is empty!" << endl;
	}
	else
	{
		printMessageHeader();
		TokenList::iterator it = m_tokenList.begin();
		for (; it != m_tokenList.end(); it++)
		{
			(*it).printTokenInfo();
		}
	}
}


void LexicalAnalysis::printLexError()
{
	if (m_errorToken.getType() != T_NO_TYPE)
	{
		printMessageHeader();
		m_errorToken.printTokenInfo();
	}
	else
	{
		cout << "There are no lexical errors!" << endl;
	}
}


void LexicalAnalysis::printMessageHeader()
{
	cout << setw(LEFT_ALIGN) << left << "Type:";
	cout << setw(RIGHT_ALIGN) << right << "Value:" << endl;
	cout << setfill('-') << setw(LEFT_ALIGN+RIGHT_ALIGN+1) << " " << endl;
	cout << setfill(' ');
}
