#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/chrono.hpp>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>

/*!
 * @example : lancé sans rien, il lance la partie serveur, qui va d'elle-même invoquer le même exe mais paramétré pour le client
 * @param argc
 * @param argv : rien par défaut
 * @return 0 si tout fonctionne parfaitement
 */
int
main(int argc, char *argv[])
{
	using namespace boost::interprocess;
	
	boost::chrono::high_resolution_clock::time_point t1, t2, t3;
	
	
	
	// Client process
	{
		std::cout << "\nclient lancé" << std::flush;
		
		t1 = boost::chrono::high_resolution_clock::now();
		//Open already created shared memory object.
		shared_memory_object shm(open_only, "MySharedMemory", read_only);
		
		//Map the whole shared memory in this process
		mapped_region region(shm, read_only);
		
		//Check that memory was initialized to 1
		char             *mem = static_cast<char *>(region . get_address());
		
		
		
		t2 = boost::chrono::high_resolution_clock::now();
		for (std::size_t i    = 0; i < region . get_size(); ++ i)
			if ( *mem ++ != 2)
				{
					std::cerr << "\nclient: shared mem , erreur valeur";
					return 1;   //Error checking memory
				}
		t3 = boost::chrono::high_resolution_clock::now();
		auto duration1 = (boost::chrono::duration_cast<boost::chrono::milliseconds>(t2-t1));
		auto duration2 = (boost::chrono::duration_cast<boost::chrono::milliseconds>(t3-t2));
		
		std::cout << "\nclient: shared mem ok, durations (ms):" << duration1.count() << " / " << duration2.count() ;
		std::cout << "\nclient: region size (bytes)" << region.get_size() << " / "  << region.get_size() / (1024*1024)  << " Mo"
		<< "\nspeed (Mo/s): " << region.get_size() / static_cast<float>(1024*1024)  * float(1000.0f / duration2.count())
		<<  std::flush;
	}
	return 0;
}
