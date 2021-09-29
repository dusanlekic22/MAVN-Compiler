/* Autor: Dusan Lekic Datum: 14.06.2020. */
#pragma once

#include "LexicalAnalysis.h"
#include "Token.h"
#include "IR.h"

typedef std::vector<std::pair<std::string, int>> Labels;

class SyntaxAnalysis
{
public:
	
	/**
	* Constructor
	*/
	SyntaxAnalysis(LexicalAnalysis& lex, Instructions& inst);

	std::vector<std::string>& getVectorOperandi();

	std::vector<std::string>& getVectorInstrukcija();
	/**
	* Method which performs lexical analysis
	*/
	bool Do();

	Variables& getRegVariables();

	Variables& getMemVariables();

	 std::vector<std::string> m_registri;

	 std::vector<std::string> m_mem;

	 std::vector<std::string> m_operandi;
	 
	 
	 std::vector<std::string> m_naziviOp;

	 std::vector<std::string> m_naziviInstr;
	 
	 std::vector<std::string> m_labele;
	 
	 std::vector<std::string> m_funk;

	 int m_instrukcijeCnt;
	 int m_promenljiveCnt;

	 std::string m_labela;
	 Labels m_labels;
	 void dodajSucc();
	 void dodajPred();
	 void CreateMIPSFile( std::string & filePath);
private:
	/**
	* Prints the error message, and token that caused the syntax error
	*/
	void printSyntaxError(Token token);
	void semanticError(int k);
	/**
	* Prints the token info
	*/
	void printTokenInfo(Token token);

	/**
	* Eats the current token if its type is "t"
	* otherwise reports syntax error
	*
	* param[in] - t - the expected token type
	*/
	void eat(TokenType t);

	/**
	* Returns the next token from the token list
	*/
	Token getNextToken();

	/**
	* Nonterminal Q
	*/
	void q();

	/**
	* Nonterminal S
	*/
	void s();

	/**
	* Nonterminal L
	*/
	void l();

	/**
	* Nonterminal E
	*/
	void e();


	//pravljenje funckije
	void memPromenljiva(Token&, Token& );
	void regPromenljiva(Token& );
	void funkcija(Token&);
	void labela(const std::string& , int );
	void napraviInstrukciju(std::vector<Token>&, std::vector<Token>&, InstructionType);
	int pozicijaSledeceInst(std::string);
	

	Variable* nadjiRegPromenljivu(const std::string& );
	Variable* nadjiMemPromenljivu(const std::string&);

	
	/**
	* Reference to lexical analysis module
	*/
	LexicalAnalysis& m_lexicalAnalysis;

	//instrukcije
	Instructions& m_instructions;

	/**
	* Syntax error indicator
	*/
	bool m_errorFound;

	/**
	* Iterator to the token list which represents the output of the lexical analysis
	*/
	TokenList::iterator m_tokenIterator;

	/**
	* Current token that is being analyzed
	*/
	Token m_currentToken;
	
	int m_linija;

	bool m_Error;
	
	Variables m_memVariables;       
	Variables m_regVariables;

	std::list<std::string> m_funkcije;

};