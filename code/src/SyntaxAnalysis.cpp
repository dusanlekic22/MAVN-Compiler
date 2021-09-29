/* Autor: Dusan Lekic Datum: 14.06.2020. */
#include "SyntaxAnalysis.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>


using namespace std;


SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex, Instructions& instrukcije) : m_lexicalAnalysis(lex), m_instructions(instrukcije),m_errorFound(false),m_Error(false),m_linija(0),
m_tokenIterator(m_lexicalAnalysis.getTokenList().begin()), m_instrukcijeCnt(0), m_promenljiveCnt(0)
{
}
vector<string>& SyntaxAnalysis::getVectorOperandi()
{
	return m_naziviOp;
}
std::vector<std::string>& SyntaxAnalysis::getVectorInstrukcija()
{
	return m_naziviInstr;
}
bool SyntaxAnalysis::Do()
{
	m_currentToken = getNextToken();

	//TO DO: Call function for the starting non-terminal symbol
	q();
	return !m_errorFound;
}

Variables & SyntaxAnalysis::getRegVariables()
{
	return m_regVariables;
}


void SyntaxAnalysis::printSyntaxError(Token token)
{
	if (m_Error)
	{
		cout << "Semanticka greska! Token: " << token.getValue() << " je neocekivan" << endl;
	}

}

void SyntaxAnalysis::semanticError(int k)
{
	if (k == 0) 
	{
		if (find(m_mem.begin(), m_mem.end(), m_currentToken.getValue()) != m_mem.end())
		{
			cout << "SEMANTICKA GRESKA NA LINIJI [" << m_linija + 1 << "] : DEKLARISANO VISE MEMORIJSKIH PROMENLJIVIH SA ISTIM IMENOM" << endl;
			m_Error = true;
			throw runtime_error("");
		}
	}
	else if( k == 1)
	{
		if (find(m_registri.begin(), m_registri.end(), m_currentToken.getValue()) != m_registri.end())
		{
			cout << "SEMANTICKA GRESKA NA LINIJI [" << m_linija + 1 << "] : DEKLARISANO VISE REGISTRA SA ISTIM IMENOM" << endl;
			m_Error = true;
			throw runtime_error("");
		}
	}
	else if(k == 2)
	{
		if (find(m_registri.begin(), m_registri.end(), m_currentToken.getValue()) == m_registri.end())
		{
			cout << "SEMANTICKA GRESKA NA LINIJI [" << m_linija + 1 << "] : NIJE DEKLARISAN REGISTAR" << endl;
			m_Error = true;
			throw runtime_error("");
		}
	}
	else if (k == 3)
	{
		if (find(m_mem.begin(), m_mem.end(), m_currentToken.getValue()) == m_mem.end())
		{
			cout << "SEMANTICKA GRESKA NA LINIJI [" << m_linija + 1 << "] : NIJE DEKLARISANA MEMORIJSKA PROMENLJIVA" << endl;
			m_Error = true;
			throw runtime_error("");
		}
	}
	else if (k == 4)
	{
		if (find(m_labele.begin(), m_labele.end(), m_currentToken.getValue()) != m_labele.end())
		{
			cout << "SEMANTICKA GRESKA NA LINIJI [" << m_linija + 1 << "] :  DEKLARISANO VISE LABELA SA ISTIM IMENOM" << endl;
			m_Error = true;
			throw runtime_error("");
		}
	}
	else if (k == 5)
	{
		if (find(m_funkcije.begin(), m_funkcije.end(), m_currentToken.getValue()) != m_funkcije.end())
		{
			cout << "SEMANTICKA GRESKA NA LINIJI [" << m_linija + 1 << "] :  DEKLARISANO VISE FUNKCIJA SA ISTIM IMENOM" << endl;
			m_Error = true;
			throw runtime_error("");
		}
	}

}


void SyntaxAnalysis::memPromenljiva(Token& t, Token& value)
{
	string variableName = t.getValue();
	Variable* variable = new Variable(variableName, Variable::T_MEM,
	  					 m_promenljiveCnt,stoi(value.getValue())) ;

	m_memVariables.push_back(variable);
}
void SyntaxAnalysis::regPromenljiva(Token& t)
{
	string variableName = t.getValue();
	Variable* variable = new Variable(variableName, Variable::T_REG,
								m_promenljiveCnt);
	++m_promenljiveCnt;

	m_regVariables.push_back(variable);
}
void SyntaxAnalysis::funkcija(Token& t)
{
	string imeFunkcije = t.getValue();

	m_funkcije.push_back(imeFunkcije);
	labela(imeFunkcije, m_instrukcijeCnt);
}
void SyntaxAnalysis::labela(const string& labelName, int pos)
{
	m_labels.push_back(pair<string, int>(labelName, pos));
	m_labela = labelName;
}
Variable* SyntaxAnalysis::nadjiRegPromenljivu(const string& varName)
{
	for (Variables::iterator it = m_regVariables.begin();
		it != m_regVariables.end();
		it++)
	{
		if ((*it)->getName() == varName)
			return (*it);
	}

	return NULL;
}
Variable* SyntaxAnalysis::nadjiMemPromenljivu(const string& varName)
{
	for (Variables::iterator it = m_memVariables.begin();
		it != m_memVariables.end();
		it++)
	{
		if ((*it)->getName() == varName)
			return (*it);
	}

	return NULL;
}

void SyntaxAnalysis::napraviInstrukciju(vector<Token>& dst, vector<Token>& src, InstructionType tip)
{
	Variables* destPromenljiva = new Variables;
	Variables* srcPromenljiva = new Variables;
	Variable* tVar;
	string imeLabele = "";
	Instruction* instr;

	switch (tip)
	{
	case I_ADD:  // add rid,rid,rid
	case I_SUB:  // sub rid,rid,rid

		tVar = nadjiRegPromenljivu(dst[0].getValue());
		destPromenljiva->push_back(tVar);
		tVar = nadjiRegPromenljivu(src[0].getValue());
		srcPromenljiva->push_back(tVar);
		tVar = nadjiRegPromenljivu(src[1].getValue());
		srcPromenljiva->push_back(tVar);

		instr = new Instruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva, imeLabele, m_labela);
		break;

	case I_ADDI: // addi rid,rid,num

		tVar = nadjiRegPromenljivu(dst[0].getValue());
		destPromenljiva->push_back(tVar);
		tVar = nadjiRegPromenljivu(src[0].getValue());
		srcPromenljiva->push_back(tVar);

		instr = new ConstInstruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva,
			stoi(src[1].getValue()), imeLabele, m_labela);
		break;

	case I_LA: // la rid,mid

		tVar = nadjiRegPromenljivu(dst[0].getValue());
		destPromenljiva->push_back(tVar);
		tVar = nadjiMemPromenljivu(src[0].getValue());
		srcPromenljiva->push_back(tVar);
		instr = new Instruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva,
			imeLabele, m_labela);
		break;

	case I_LW: // lw rid,num(rid)
		tVar = nadjiRegPromenljivu(dst[0].getValue());
		destPromenljiva->push_back(tVar);

		tVar = nadjiRegPromenljivu(src[1].getValue());
		srcPromenljiva->push_back(tVar);

		instr = new ConstInstruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva,
			stoi(src[0].getValue()), imeLabele, m_labela);
		break;
	case I_SW: // sw rid,num(rid)
	case I_SB:

		tVar = nadjiRegPromenljivu(dst[0].getValue());
		srcPromenljiva->push_back(tVar);

		tVar = nadjiRegPromenljivu(src[1].getValue());
		srcPromenljiva->push_back(tVar);

		instr = new ConstInstruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva,
			stoi(src[0].getValue()), imeLabele, m_labela);
		break;

	case I_LI: // li rid,num

		tVar = nadjiRegPromenljivu(dst[0].getValue());
		destPromenljiva->push_back(tVar);

		instr = new ConstInstruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva,
			stoi(src[0].getValue()), imeLabele, m_labela);
		break;

	case I_BLTZ: // bltz rid,id

		tVar = nadjiRegPromenljivu(src[0].getValue());
		srcPromenljiva->push_back(tVar);

		imeLabele = dst[0].getValue();


		instr = new Instruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva,
			imeLabele, m_labela);
		break;

	case I_B:   // b id
		imeLabele = dst[0].getValue();


		instr = new Instruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva,
			imeLabele, m_labela);
		break;

	case I_NOP: // nop
		instr = new Instruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva,
			imeLabele, m_labela);
		break;

	case I_DIV: // div
		tVar = nadjiRegPromenljivu(dst[0].getValue());
		destPromenljiva->push_back(tVar);
		tVar = nadjiRegPromenljivu(src[0].getValue());
		srcPromenljiva->push_back(tVar);
		tVar = nadjiRegPromenljivu(src[1].getValue());
		srcPromenljiva->push_back(tVar);

		instr = new Instruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva, imeLabele, m_labela);
		break;
	case I_XOR: // xor
		tVar = nadjiRegPromenljivu(dst[0].getValue());
		destPromenljiva->push_back(tVar);
		tVar = nadjiRegPromenljivu(src[0].getValue());
		srcPromenljiva->push_back(tVar);
		tVar = nadjiRegPromenljivu(src[1].getValue());
		srcPromenljiva->push_back(tVar);

		instr = new Instruction(m_instrukcijeCnt, tip, *destPromenljiva, *srcPromenljiva, imeLabele, m_labela);
		break;
	}
	++m_instrukcijeCnt;
	m_instructions.push_back(instr);

	
}

int SyntaxAnalysis::pozicijaSledeceInst(std::string label)
{
	for (Labels::iterator it = m_labels.begin();
		it != m_labels.end();
		it++)
	{
		if (it->first == label)
			return it->second;
	}

	return -1;
}

void SyntaxAnalysis::dodajSucc()
{
	Instructions::iterator itn;
	for (Instructions::iterator it = m_instructions.begin();
		it != (m_instructions.end()-1);
		it++)
	{
		itn = it;
		itn++;
		// Instruction B has only 1 succ
		if ((*it)->getType() != I_B)
			(*it)->m_succ.push_back(*itn);
			
		// If instr. is BRANCH type then we need to add one more successor
		if ((*it)->getType() == I_B || (*it)->getType() == I_BLTZ)
		{
			int instrPosition = pozicijaSledeceInst((*it)->getLabelName());
			Instruction* instrToJump = m_instructions.at(instrPosition);
			(*it)->m_succ.push_back(instrToJump);

			// Add predecessor for instr. we jump to
			instrToJump->m_pred.push_back(*itn);
		}
	}
}

void SyntaxAnalysis::dodajPred()
{
	Instructions::reverse_iterator itp;
	for (Instructions::reverse_iterator it = m_instructions.rbegin();
		it != m_instructions.rend()-1 ;
		it++)
	{
		itp=it;
		itp++;
		(*it)->m_pred.push_back(*itp);
	}
}


void SyntaxAnalysis::printTokenInfo(Token token)
{
	 token.printTokenInfo() ; 
}


void SyntaxAnalysis::eat(TokenType t)
{
	if (m_errorFound == false)
	{
		if (m_currentToken.getType() == t)
		{
			
			if( m_currentToken.getType() > 7 && m_currentToken.getType() < 22)
			{
				m_naziviInstr.push_back(m_currentToken.getValue());
			}
			else 
			{
				m_naziviOp.push_back(m_currentToken.getValue());
			}
			cout << m_currentToken.getValue() << endl;
			if (m_tokenIterator != m_lexicalAnalysis.getTokenList().end())
			{
				m_currentToken = getNextToken(); // trazi novi token ako nisi na kraju
			}
		}
		else
		{
			printSyntaxError(m_currentToken);
			m_errorFound = true;
		}
	}
}


Token SyntaxAnalysis::getNextToken()
{	
	return *m_tokenIterator++;
}


void SyntaxAnalysis::q()
{
	
	if (m_errorFound) return;

		s();
		if (m_currentToken.getType() == T_SEMI_COL)
		{
			m_linija++;
			eat(T_SEMI_COL);
		}
		else 
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
		l();
}


void SyntaxAnalysis::s()
{
	
	if (m_errorFound) return;

	if (m_currentToken.getType() == T_MEM)
	{
		eat(T_MEM);
		if (m_currentToken.getType() == T_M_ID) 
		{
			Token temp;
			//sematicka greska dvostruka deklaracija
			semanticError(0);
			m_mem.push_back(m_currentToken.getValue());
			temp = m_currentToken;
			eat(T_M_ID);
			if (m_currentToken.getType() == T_NUM)
			{
				memPromenljiva(temp,m_currentToken);
				eat(T_NUM);
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_REG)
	{
		eat(T_REG);
		if (m_currentToken.getType() == T_R_ID)
		{
			//sematicka greska dvostruka deklaracija
			semanticError(1);
			m_registri.push_back(m_currentToken.getValue());
			regPromenljiva(m_currentToken);
			eat(T_R_ID);
		}
		else 
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_FUNC) 
	{
		m_funk.push_back(m_currentToken.getValue());
		eat(T_FUNC);
		if (m_currentToken.getType() == T_ID) 
		{
			semanticError(5);
			funkcija(m_currentToken);
			eat(T_ID);
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_ID)
	{
		string lIme;
		semanticError(4);
		lIme = m_currentToken.getValue();
		m_labele.push_back(m_currentToken.getValue());
		eat(T_ID);
		if (m_currentToken.getType() == T_COL)
		{
			labela(lIme,m_instrukcijeCnt);
			eat(T_COL);
			e();
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else
	{
		e();
	}



}


void SyntaxAnalysis::l()
{
	
	if (m_errorFound) return;

	if (m_currentToken.getType() == T_END_OF_FILE)
	{
		eat(T_END_OF_FILE);
	}
	else 
	{
		q();
	}

}


void SyntaxAnalysis::e()
{
	vector<Token> src;

	vector<Token> dst;

	if (m_errorFound) return;

	if (m_currentToken.getType() == T_ADD)
	{
		eat(T_ADD);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_R_ID)
				{
					semanticError(2);
					src.push_back(m_currentToken);
					eat(T_R_ID);
					if (m_currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (m_currentToken.getType() == T_R_ID)
						{
							semanticError(2);
							src.push_back(m_currentToken);
							eat(T_R_ID);
							napraviInstrukciju(dst, src, I_ADD);
						}
						else
						{
							m_errorFound = true;
							printSyntaxError(m_currentToken);
						}
					}
					else
					{
						m_errorFound = true;
						printSyntaxError(m_currentToken);
					}
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_DIV)
	{
		eat(T_DIV);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_R_ID)
				{
					semanticError(2);
					src.push_back(m_currentToken);
					eat(T_R_ID);
					if (m_currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (m_currentToken.getType() == T_R_ID)
						{
							semanticError(2);
							src.push_back(m_currentToken);
							eat(T_R_ID);
							napraviInstrukciju(dst, src, I_DIV);
						}
						else
						{
							m_errorFound = true;
							printSyntaxError(m_currentToken);
						}
					}
					else
					{
						m_errorFound = true;
						printSyntaxError(m_currentToken);
					}
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_XOR)
	{
		eat(T_XOR);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_R_ID)
				{
					semanticError(2);
					src.push_back(m_currentToken);
					eat(T_R_ID);
					if (m_currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (m_currentToken.getType() == T_R_ID)
						{
							semanticError(2);
							src.push_back(m_currentToken);
							eat(T_R_ID);
							napraviInstrukciju(dst, src, I_XOR);
						}
						else
						{
							m_errorFound = true;
							printSyntaxError(m_currentToken);
						}
					}
					else
					{
						m_errorFound = true;
						printSyntaxError(m_currentToken);
					}
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_ADDI)
	{
		eat(T_ADDI);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_R_ID)
				{
					semanticError(2);
					src.push_back(m_currentToken);
					eat(T_R_ID);
					if (m_currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (m_currentToken.getType() == T_NUM)
						{
							src.push_back(m_currentToken);
							eat(T_NUM);
							napraviInstrukciju(dst, src, I_ADDI);
						}
						else
						{
							m_errorFound = true;
							printSyntaxError(m_currentToken);
						}
					}
					else
					{
						m_errorFound = true;
						printSyntaxError(m_currentToken);
					}
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_SUB)
	{
		eat(T_SUB);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_R_ID)
				{
					semanticError(2);
					src.push_back(m_currentToken);
					eat(T_R_ID);
					if (m_currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (m_currentToken.getType() == T_R_ID)
						{
							semanticError(2);
							src.push_back(m_currentToken);
							eat(T_R_ID);
							napraviInstrukciju(dst, src, I_SUB);
						}
						else
						{
							m_errorFound = true;
							printSyntaxError(m_currentToken);
						}
					}
					else
					{
						m_errorFound = true;
						printSyntaxError(m_currentToken);
					}
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_LA)
	{
		eat(T_LA);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_M_ID)
				{
					semanticError(3);
					src.push_back(m_currentToken);
					eat(T_M_ID);
					napraviInstrukciju(dst, src, I_LA);
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}

	}
	else if (m_currentToken.getType() == T_LW)
	{
		eat(T_LW);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_NUM)
				{
					src.push_back(m_currentToken);
					eat(T_NUM);
					if (m_currentToken.getType() == T_L_PARENT)
					{
						eat(T_L_PARENT);
						if (m_currentToken.getType() == T_R_ID)
						{
							semanticError(2);
							src.push_back(m_currentToken);
							eat(T_R_ID);
							if (m_currentToken.getType() == T_R_PARENT)
							{
								eat(T_R_PARENT);
								napraviInstrukciju(dst, src, I_LW);
							}
							else
							{
								m_errorFound = true;
								printSyntaxError(m_currentToken);
							}
						}
						else
						{
							m_errorFound = true;
							printSyntaxError(m_currentToken);
						}
					}
					else
					{
						m_errorFound = true;
						printSyntaxError(m_currentToken);
					}
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if(m_currentToken.getType() == T_LI)
	{
		eat(T_LI);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_NUM)
				{
					src.push_back(m_currentToken);
					eat(T_NUM);
					napraviInstrukciju(dst, src, I_LI);
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_SW)
	{
		eat(T_SW);
		
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_NUM)
				{
					src.push_back(m_currentToken);
					eat(T_NUM);
					if (m_currentToken.getType() == T_L_PARENT)
					{
						eat(T_L_PARENT);
						if (m_currentToken.getType() == T_R_ID)
						{
							semanticError(2);
							src.push_back(m_currentToken);
							eat(T_R_ID);
							if (m_currentToken.getType() == T_R_PARENT)
							{
								eat(T_R_PARENT);
								napraviInstrukciju(dst, src, I_SW);
							}
							else
							{
								m_errorFound = true;
								printSyntaxError(m_currentToken);
							}
						}
						else
						{
							m_errorFound = true;
							printSyntaxError(m_currentToken);
						}
					}
					else
					{
						m_errorFound = true;
						printSyntaxError(m_currentToken);
					}
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_SB)
	{
		eat(T_SB);

		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			dst.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_NUM)
				{
					src.push_back(m_currentToken);
					eat(T_NUM);
					if (m_currentToken.getType() == T_L_PARENT)
					{
						eat(T_L_PARENT);
						if (m_currentToken.getType() == T_R_ID)
						{
							semanticError(2);
							src.push_back(m_currentToken);
							eat(T_R_ID);
							if (m_currentToken.getType() == T_R_PARENT)
							{
								eat(T_R_PARENT);
								napraviInstrukciju(dst, src, I_SB);
							}
							else
							{
								m_errorFound = true;
								printSyntaxError(m_currentToken);
							}
						}
						else
						{
							m_errorFound = true;
							printSyntaxError(m_currentToken);
						}
					}
					else
					{
						m_errorFound = true;
						printSyntaxError(m_currentToken);
					}
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_B)
	{
		eat(T_B);
		if (m_currentToken.getType() == T_ID)
		{
			dst.push_back(m_currentToken);
			eat(T_ID);
			napraviInstrukciju(dst, src, I_B);
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else if (m_currentToken.getType() == T_BLTZ)
	{
		eat(T_BLTZ);
		if (m_currentToken.getType() == T_R_ID)
		{
			semanticError(2);
			src.push_back(m_currentToken);
			eat(T_R_ID);
			if (m_currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (m_currentToken.getType() == T_ID)
				{
					dst.push_back(m_currentToken);
					eat(T_ID);
					napraviInstrukciju(dst, src, I_BLTZ);
				}
				else
				{
					m_errorFound = true;
					printSyntaxError(m_currentToken);
				}
			}
			else
			{
				m_errorFound = true;
				printSyntaxError(m_currentToken);
			}
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}
	else
	{
		if (m_currentToken.getType() == T_NOP)
		{
			eat(T_NOP);
			napraviInstrukciju(dst, src, I_NOP);
		}
		else
		{
			m_errorFound = true;
			printSyntaxError(m_currentToken);
		}
	}

}

void SyntaxAnalysis::CreateMIPSFile( std::string& filePath)
{
	ofstream outFile(filePath);

	// Fill .globl section with functions
	for (list<string>::iterator it = m_funkcije.begin(); it != m_funkcije.end(); it++)
		outFile << ".globl " << *it << "\n";

	// Fill .data section with memory variables and it's values
	outFile << "\n.data\n";

	for (Variables::iterator it = m_memVariables.begin();
		it != m_memVariables.end();
		it++)
	{
		outFile << (*it)->getName() << ": " << ".word "
			<< (*it)->getValue() << "\n";
	}

	// Fill .text section with instructions and labels
	outFile << "\n.text\n";


	// Print instructons by labels
	// (Every instructions contains in which label it should be)
	Instructions::iterator instr = m_instructions.begin();
	for (Labels::iterator it = m_labels.begin();
		it != m_labels.end();
		it++)
	{
		outFile << it->first << ":\n";

		// Print instructions for label *it
		while (instr != m_instructions.end())
		{
			// Exit condition, instruction is in another label
			if ((*instr)->getLabel() != it->first)
			{
				break;
			}

			outFile << "\t" << *(*instr) << "\n";
			cout << "\t" << *(*instr) << "\n";
			instr++;
		}
	}

	outFile.close();
	
}
