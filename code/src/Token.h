/* Autor: Dusan Lekic Datum: 14.06.2020. */
#ifndef __TOKEN__
#define __TOKEN__

#include <vector>
#include <string>

#include "Constants.h"
#include "Types.h"


class Token
{
public:
	Token() : m_tokenType(T_NO_TYPE), m_value("") {}

	/**
	 * Returns token type
	 */
	TokenType getType();

	/**
	 * Sets token type
	 */
	void setType(TokenType t);

	/**
	 * Returns token value
	 */
	std::string getValue();

	/**
	 * Sets token value
	 */
	void setValue(std::string s);

	/**
	 * Creates a token
	 * [in] begin - start position in the program buffer - first character of the token
	 * [in] end - end position in the program buffer - end character of the token
	 * [in] program - program buffer
	 * [in] lastFiniteState - number of the last finite state,
	 *		this is used to get the name of the state and store it as token type
	 */
	void makeToken(int begin, int end, std::vector<char>& program, int lastFiniteState);

	/**
	 * Creates an error token, storing the errnous content as token value
	 */
	void makeErrorToken(int pos, std::vector<char>& program);

	/**
	 * Creates end of file token when it is reached
	 */
	void makeEofToken();

	/**
	 * Prints token type and value
	 */
	void printTokenInfo();

	/**
	 * Prints token value
	 */
	void printTokenValue();
	
private:
	/**
	 * Type of the token - as enumeration (defined in common.h)
	 */
	TokenType m_tokenType;
	
	/**
	 * Value of the token - as string
	 */
	std::string m_value;
	/**
	* Helper function to get string representation of token type
	*/
	std::string tokenTypeToString(TokenType t);
	
};

#endif
