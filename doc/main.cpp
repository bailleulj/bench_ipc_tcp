#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/chrono.hpp>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>

/*!
 * @author https://www.boost.org/doc/libs/1_62_0/doc/html/interprocess/sharedmemorybetweenprocesses.html
 * @example : lancé sans rien, il lance la partie serveur, qui va d'elle-même invoquer le même exe mais paramétré pour le client
 * @param argc
 * @param argv : rien par défaut
 * @return 0 si tout fonctionne parfaitement
 */
int
main(int argc, char *argv[])
{
	using namespace boost::interprocess;
	
	if ( argc == 1 ) //Parent process
		{
			std::cout << "\nparent lancé" << std::flush;
			
			//Remove shared memory on construction and destruction
			struct shm_remove
			{
			  shm_remove()
			  { shared_memory_object::remove("MySharedMemory"); }
			  ~shm_remove()
			  { shared_memory_object::remove("MySharedMemory"); }
			};
			struct shm_remove remover;
			
			//Create a shared memory object.
			shared_memory_object shm(create_only, "MySharedMemory", read_write);
			
			//Set size
			shm . truncate(1000);
			
			//Map the whole shared memory in this process
			mapped_region region(shm, read_write);
			
			//Write all the memory to 1
			std::memset(region . get_address(), 1, region . get_size());
			
			//Launch child process
			std::string s(argv[0]);
			s += " child ";
			if ( 0 != std::system(s . c_str()))
				return 1;
		}
	else // Client process
		{
			std::cout << "\nclient lancé" << std::flush;
			
			//Open already created shared memory object.
			shared_memory_object shm(open_only, "MySharedMemory", read_only);
			
			//Map the whole shared memory in this process
			mapped_region region(shm, read_only);
			
			//Check that memory was initialized to 1
			char             *mem = static_cast<char *>(region . get_address());
			for (std::size_t i    = 0; i < region . get_size(); ++ i)
				if ( *mem ++ != 1 )
					return 1;   //Error checking memory
		}
	return 0;
}