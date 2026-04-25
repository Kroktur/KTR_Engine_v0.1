

#include <iostream>

#include "KTR_Application.h"
#include "Containers/KTR_Sparse.h"
#include "Tools/KTR_RTTI.h"

#include "Tools/KTR_BiteSet.h"
#include "Tools/KTR_Random.h"
#include "Tools/KTR_Chrono.h"
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
	KTR::TIME::Timer<float> timer(KTR::TIME::Now(),false);
	
	KTR::BitSet<std::uint32_t> bite;
	KTR::Random rd(100);
	std::cout << rd.getRandom(1, 4) << std::endl;;
	std::cout << rd.getRandom(1, 4) << std::endl;;
	KTR::Random rd2(100);
	std::cout << rd2.getRandom(1, 4) << std::endl;
	std::cout << rd2.getRandom(1, 4) << std::endl;
	KTR::TIME::Time<float> time = KTR::TIME::Time<float>::FromValue<KTR::TIME::seconds_type>(10);

	std::cout << time.AsRatio<KTR::TIME::seconds_type>() << std::endl;;
	std::cout << time.AsRatio<KTR::TIME::milli_seconds_type>() << std::endl;;
	std::cout << time.AsRatio<KTR::TIME::micro_seconds_type>() << std::endl;;
	std::cout << time.AsRatio<KTR::TIME::nano_seconds_type>() << std::endl;;


	KTR::TIME::Time<float> time2;
	std::cout << time2.AsRatio<KTR::TIME::nano_seconds_type>() << std::endl;;
	std::cout << timer.GetTime().AsRatio<KTR::TIME::seconds_type>() << std::endl;

	KTR_APPLICATION_END;


}
