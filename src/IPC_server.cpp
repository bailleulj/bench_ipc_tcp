// from https://www.boost.org/doc/libs/1_45_0/doc/html/interprocess/quick_guide.html

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/program_options.hpp>

#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>


/// structure remplie par ligne de commande
struct progArgs
{
  std::string sm_stringname; // shared memory identifier
  size_t      sm_size_bytes; // payload size in octets / Bytes
  size_t      sm_size_mega;  // same in Mo / Mb : ignoré par la suite
  char        value; // valeur unique de tous les élts de la payload
};

static void usage(int argc, char **argv, progArgs &a);

const char *sm_stringnamestr_g; /// global contenant l'identifiant de la shared memory


/*!
 * @return 0 si tout fonctionne parfaitement
 */
int
main(int argc, char *argv[])
{
	// usage & argument parsing
	progArgs args;
	usage(argc, argv, args);
	
	sm_stringnamestr_g = args . sm_stringname . c_str();
	
	//Parent process
	{
		using namespace boost::interprocess;
		
		std::cout << "\nparent launched (SM name=" << sm_stringnamestr_g << ")" << std::flush;
		
		//Remove shared memory on construction and destruction
		struct shm_remove
		{
		  shm_remove()
		  { shared_memory_object::remove(sm_stringnamestr_g); }
		  ~shm_remove()
		  { shared_memory_object::remove(sm_stringnamestr_g); }
		};
		struct shm_remove remover;
		
		//Create a shared memory object.
		shared_memory_object shm(create_only, sm_stringnamestr_g, read_write);
		
		//Set size
		shm . truncate(args . sm_size_bytes * sizeof(char));
		
		//Map the whole shared memory in this process
		mapped_region region(shm, read_write);
		
		//Write all the memory to value
		std::memset(region . get_address(), args . value, region . get_size());
		
		std::cout << "IPC server: pool of size " << args.sm_size_bytes << "octets/Bytes or" <<
		static_cast<float>(args.sm_size_bytes) / (1024*1024) << "MB/Mo" << std::flush;
		

		// assurer la persistance de la mémoire partagée
		size_t c = 0;
		do
			{
				boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
			}
		while (++c > 0);
		
	}
	return 0;
}


// ==================================================================

/*!
 * @brief usage: affiche aide par défaut en ligne de commande si aucun argument.
 * Et fait le lien entre les arguments saisis et la struct progArgs
 * @param argc
 * @param argv
 * @param a
 */
static void
usage(int argc, char **argv, progArgs &a)
{
	// initial values
	a . sm_size_bytes = 1024 * 1024;
	
	// let the parsing commence
	namespace po = boost::program_options;
	po::options_description desc("This program lance un serveur IPC qui va transférer, à tout client, une payload de taille définie en argt");
	
	desc . add_options()
		     ("help,h", "Print help messages")    // no space after comma
		     ("sharedtag,t", po::value<std::string>(&a . sm_stringname) -> required(), \
             "Define shared memory name/tag")
		     ("sizeMB", po::value<size_t>(&a . sm_size_mega), \
                 "Set payload size in megaBytes/méga-octets")
		     ("sizeB", po::value<size_t>(&a . sm_size_bytes), \
             "Set payload size in bytes/octets")
		     ("value,v", po::value<char>(&a . value) -> default_value(3), \
                 "Value of all bytes in payload");
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc),
	          vm);
	
	if ( vm . count("help"))
		{
			std::cout << desc << "This is the basic help message\n";
			exit(EXIT_SUCCESS);
		}
	
	
	try
		{
			po::notify(vm); // will throw if a required option is not set: usually, call help first
		}
	catch (po::error &e)
		{
			std::cerr << desc;
			exit(EXIT_FAILURE);  // show usage even if args broke
		}
	
	
	if ( vm . count("sizeMB"))
		a . sm_size_bytes = a . sm_size_mega * 1024 * 1024;
}

