/* Autor: Dusan Lekic Datum: 14.06.2020. */
#include "Types.h"

/* Prints register to the output. */
std::ostream& operator<<(std::ostream& out, Regs r)
{
	switch (r)
	{
	case t0: out << "t0"; break;
	case t1: out << "t1"; break;
	case t2: out << "t2"; break;
	case t3: out << "t3"; break;
	case t4: out << "t4"; break;
	}

	return out;
}