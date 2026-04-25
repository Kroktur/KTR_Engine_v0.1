

#include <iostream>

#include "KTR_application.h"
#include "KTR_Sparse.h"
#include "KTR_RTTI.h"

#include "KTR_BiteSet.h"

enum  test : std::uint32_t
{
	toto = 1 << 0,
	tata = 1<< 1,
	titi = 1 << 2
};

REGISTER_RTTI_TYPE(KTR::Sparse<std::uint32_t>);
REGISTER_RTTI_TYPE(int);


KTR_ARGV_APPLICATION
{
	KTR::BitSet<std::uint32_t> bite;

	int toto = 0;
	toto &= ~0;

	bite.Add(KTR::Bit<std::uint32_t>::EnumToBit<test,true>(titi)) ;
	auto b = bite;
	b.Toggle(1 << 2);
	b.Remove(1 << 3);
	auto b2 = bite - b;
	std::cout << (b2.Has(1<<2))  ;


	KTR_APPLICATION_END;


}
