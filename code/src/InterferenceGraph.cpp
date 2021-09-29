/* Autor: Dusan Lekic Datum: 14.06.2020. */
#include "InterferenceGraph.h"

#include <iomanip>
#include "Constants.h"

using namespace std;

InterferenceGraph::InterferenceGraph(Variables& vars)
	: m_regVariables(vars)
{
}

InterferenceGraph::~InterferenceGraph()
{
}

/* Creates interference graph (matrix) from instructions. */
void InterferenceGraph::buildInterferenceGraph(Instructions& instructions)
{
	// Initialize interference matrix to the size*size of reg vars
	resizeInterferenceMatrix(m_regVariables.size());

	// In every instructions, for all defined variables add
	// interference between defined variable and all out
	// variables from instruction.
	for (Instructions::iterator it = instructions.begin();
		it != instructions.end();
		it++)
	{
		for (Variables::iterator defVar = (*it)->m_def.begin();
			defVar != (*it)->m_def.end();
			defVar++)
		{
			for (Variables::iterator outVar = (*it)->m_out.begin();
				outVar != (*it)->m_out.end();
				outVar++)
			{
				int defPos = (*defVar)->getPos();
				int outPos = (*outVar)->getPos();

				// Variable can't be in interference with itself
				if (defPos != outPos)
				{
					m_im[defPos][outPos] = __INTERFERENCE__;
					m_im[outPos][defPos] = __INTERFERENCE__;
				}
				else
				{
					m_im[defPos][outPos] = __EMPTY__;
					m_im[outPos][defPos] = __EMPTY__;
				}
			}
		}
	}
}

/* Resizes matrix to the passed size. */
void InterferenceGraph::resizeInterferenceMatrix(size_t size)
{
	m_im.resize(size);

	for (unsigned i = 0; i < m_im.size(); ++i)
		m_im[i].resize(size);
}

/* Creates stack from list of register variables. */
void InterferenceGraph::buildVariableStack(int brRegistri)
{
	interferenceMatrix matiricaSmetnji = m_im; //kopija matrice
	Variables registri = m_regVariables;		 //kopija registara

	int poz = -1;
	int maxSmetnji;

	while (!registri.empty())
	{
		bool hSmetnja = false;

		maxSmetnji = -1;
		for (unsigned i = 0; i < matiricaSmetnji.size(); i++)
		{
			int interference = 0;
			for (unsigned j = 0; j < matiricaSmetnji.size(); j++)
			{
				if (matiricaSmetnji[i][j] == __INTERFERENCE__) interference++;
			}
			if (interference > maxSmetnji  && interference < brRegistri)
			{
				poz = i;
				hSmetnja = true;
			}

		}
		if (hSmetnja)
		{
			matiricaSmetnji.erase(matiricaSmetnji.begin() + poz);
			for (unsigned i = 0; i < matiricaSmetnji.size(); ++i)
				matiricaSmetnji[i].erase(matiricaSmetnji[i].begin() + poz);

			for (Variables::iterator it = registri.begin();
				it != registri.end();
				it++)
			{
				if ((*it)->getPos() == poz)
				{
					m_varStack.push(*it);
					registri.erase(it);
					break;
				}
			}
		}

		if (!hSmetnja) cout << "NEMA smetnji";

	}
}

/* Applies reg to the variable that has varPos for it's position. */
void InterferenceGraph::applyRegToVariable(int varPos, Regs reg)
{
	for (Variables::iterator it = m_regVariables.begin();
		it != m_regVariables.end();
		it++)
	{
		if ((*it)->getPos() == varPos)
			(*it)->setAssignment(reg);
	}
}
typedef list<Variable*>::iterator ITV;
typedef list<Instruction*>::iterator ITI;
/* Allocates real registers to variables according to the interference. */
bool InterferenceGraph::resourceAllocation()
{	
	
		list<Regs> registri; //boje
		list<Variable*>  promenljive;

		for (int i = 0; i < __REG_NUMBER__; i++) {
			registri.push_back((Regs)i); //boje  r0 -r3
		}

		while (!m_varStack.empty())
		{
			Variable* vrh = m_varStack.top();
			bool smetnja = false;

			for (ITV it = promenljive.begin(); it != promenljive.end(); it++) {
				if (m_im[vrh->getPos()][(*it)->getPos()] == __INTERFERENCE__) {
					smetnja = true;
					registri.remove((*it)->getAssignment());
					if (registri.empty())
						return false;
				}
			}

			if (!smetnja) {
				if (registri.empty())
					return false;
				vrh->m_assignment = registri.front();
			}
			else {
				if (!registri.empty()) {
					vrh->m_assignment = registri.front();
				}
				else { return false; }
			}
			promenljive.push_back(vrh);  //obojeni
			m_varStack.pop(); //skini promenljivu sa steka
		}


		return true;

}

/* Prints interference matrix to the console. */
void InterferenceGraph::printInterferenceMatrix()
{
	// Print first row (names of registers) as header
	printf("   ");
	for (size_t i = 0; i < m_im.size(); ++i)
		printf("r%-3d", i);

	printf("\n");

	// Print first column (names of registers) and actual data
	for (size_t i = 0; i < m_im.size(); ++i)
	{
		printf("r%d ", i);

		for (size_t j = 0; j < m_im[i].size(); ++j)
			printf("%-4d", m_im[i][j]);

		printf("\n");
	}
}