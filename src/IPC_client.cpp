// from https://www.boost.org/doc/libs/1_45_0/doc/html/interprocess/quick_guide.html

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/chrono.hpp>
#include <boost/program_options.hpp>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>


struct progArgs
{
  std::string sm_stringname; // shared memory identifier
  char        value;         // valeur unique de tous les élts de la payload définie côté serveur. vérifier que bien reçue
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
	
	
	// Client process
	{
		using namespace boost::interprocess;
		
		boost::chrono::high_resolution_clock::time_point t1, t2, t3;
		
		
		std::cout << "\nclient launched" << std::flush;
		
		//Open already created shared memory object.
		try
			{
				const char expected_val = args . value;
				
				t1 = boost::chrono::high_resolution_clock::now();
				shared_memory_object shm(open_only, sm_stringnamestr_g, read_only);
				
				//Map the whole shared memory in this process
				mapped_region region(shm, read_only);
				
				const char *mem = reinterpret_cast<const char *>( region . get_address());
				// char *mem = static_cast<char *>(region . get_address());

#if 0
				// exemple original: copie et vérification simultanées
				
				//Check that memory was initialized to same value as defined server-side
				t2 = boost::chrono::high_resolution_clock::now();
				for (std::size_t i = 0; i < region . get_size(); ++ i)
					if ( *mem ++ != expected_val )
						{
							std::cerr << "\nclient: shared mem; erreur valeur";
							return 1;   //Error checking memory
						}
				t3 = boost::chrono::high_resolution_clock::now();
#else
				char *data_copy = static_cast<char *>( malloc(region . get_size()));
				
				// on sépare la copie du reste afin d'être raccords avec l'expérience TCP
				t2 = boost::chrono::high_resolution_clock::now();
				memcpy(data_copy, mem, region . get_size());
				t3 = boost::chrono::high_resolution_clock::now();
				
				for (std::size_t i = 0; i < region . get_size(); ++ i)
					if ( *data_copy ++ != expected_val )
						{
							std::cerr << "\nclient: shared mem; erreur valeur";
							return 1;   //Error checking memory
						}
#endif
				
				auto duration1 = (boost::chrono::duration_cast<boost::chrono::microseconds>(t2 - t1));
				auto duration2 = (boost::chrono::duration_cast<boost::chrono::microseconds>(t3 - t2));
				
				std::cout << "\nclient: shared mem ok, durations (µs):" << duration1 . count() << " (init sm) / "
				          << duration2 . count() << " (copy sm payload)";
				float bandwidth = region . get_size() / static_cast<float>(1024 * 1024) * (1000.0f * 1000 / duration2 . count());
				std::cout << "\nclient: region size: " << region . get_size() << " bytes / " << region . get_size() / (1024 * 1024) << " Mo"
				          << "\nbandwidth: " << bandwidth << "(Mo/s)"
				          << std::flush;
			}
		catch (...)
			{
				std::cerr << "\ncant find shared memory bank: " << sm_stringnamestr_g;
				return 1;
			}
	}
	return 0;
}


// ========================================================

/*!
 *@brief usage: affiche aide (pas d'argts) / effectue transfert arguments vers progArgs
 */
static void
usage(int argc, char **argv, progArgs &a)
{
	// let the parsing commence
	namespace po = boost::program_options;
	po::options_description desc("This program lance un client IPC qui va demander une payload de taille définie côté serveur. On vérifiera que la "
	                             "valeur stockée est la même que côté serveur");
	
	desc . add_options()
		     ("help,h", "Print help messages")    // no space after comma
		     ("sharedtag,t", po::value<std::string>(&a . sm_stringname) -> required(), \
             "Define shared memory name/tag")
		     ("value,v", po::value<char>(&a . value) -> default_value(3), \
                 "Value of all bytes in payload set by server: we must check received payload is received as expected");
	
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
	
}
