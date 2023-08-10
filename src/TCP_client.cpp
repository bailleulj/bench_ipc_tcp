//
// Created by j.bailleul on 09/08/2023 from:
// https://gwenael-dunand.developpez.com/tutoriels/cpp/boost/asio/
//

#include <boost/program_options.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/chrono.hpp>


/// structure remplie par ligne de commande
struct progArgs
{
  std::string server_ipv4; // ipv4 adress string for server
  size_t      server_port; // port number
  size_t      tcp_packet_size; // taille des paquets TCP qui vont découper la payload à transmettre
  char        value; // valeur unique de tous les élts de la payload
};

static void usage(int argc, char **argv, progArgs &a);

const char *srv_ipv4_g; /// global contenant


int main(int argc, char **argv)
{
	// usage & argument parsing
	progArgs args;
	usage(argc, argv, args);
	
	
	boost::chrono::high_resolution_clock::time_point t1, t2, t3;
	
	
	// Création du service principal et du résolveur.
	boost::asio::io_service ios;
	
	using namespace boost::asio;
	using ip::tcp;
	
	// On veut se connecter sur la machine locale, port 7171
	tcp::endpoint endpoint(boost::asio::ip::address::from_string(args . server_ipv4), args . server_port);
	
	// On crée une socket // (1)
	tcp::socket socket(ios);
	
	// Tentative de connexion, bloquante // (2)
	socket . connect(endpoint);
	
	// Création du buffer de réception (d'un paquet) // (3)
	const size_t           buffer_size = args . tcp_packet_size;
	boost::array<char, 64> buf;
	void                   *buffer_data = malloc(buffer_size);
	// message agrégé (paquets collés ensemble)
	void* data_msg = _aligned_malloc(1024*1024*512* sizeof(char), 1024); // taille totale inconnue, maximum gérée = 512Mo;
	if (data_msg == nullptr || buffer_data == nullptr) return 1;
	
	// on découpe le message en série de buffers de buffer_size
	t1 = boost::chrono::high_resolution_clock::now();
	size_t total_len = 0;
	void* write_position = data_msg;
	
	while (1)
		{
			boost::system::error_code error;
			// Réception des données. len : nombre d'octets reçus // (4)
			size_t len = socket . read_some(boost::asio::buffer(buffer_data, buffer_size), error);
			// read_some(boost::asio::buffer(buf), error);
			
			// agrégation au message (si paquets consécutifs)
			memcpy(write_position, buffer_data, buffer_size);
			char* wrt_pos = static_cast<char *>(write_position);
			wrt_pos += buffer_size;
			write_position = static_cast<void*>(wrt_pos);
			total_len += len;
			
			if ( error == boost::asio::error::eof ) // (5)
				{
					std::cout << "\nTermine !" << std::endl;
					break;
				}

#if 0
			// On affiche (6)
			std::cout << "\npayload size:" << len << "\nmessage lu du serveur:";
			if (len < 64)
				std::cout.write(buf.data(), len);
#endif
		}
		
	// mesure à la fin de réception des données
	t2 = boost::chrono::high_resolution_clock::now();
	auto duration1 = (boost::chrono::duration_cast<boost::chrono::microseconds>(t2 - t1));
	std::cout << "\nclient: durations (µs):" << duration1 . count();
	
	float payload_mb = static_cast<float>(total_len) / (1024 * 1024);
	float bandwidth  = payload_mb * (1000 * 1000.0f / duration1 . count());
	
	std::cout << "\nclient: bandwidth (Mo/s): " << bandwidth << " (packet size: " << buffer_size << ")";
	
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
	
	
	// let the parsing commence
	namespace po = boost::program_options;
	po::options_description desc("This program lance un serveur IPC qui va transférer, à tout client, une payload de taille définie en argt");
	
	desc . add_options()
		     ("help,h", "Print help messages")    // no space after comma
		     ("srvip,i", po::value<std::string>(&a . server_ipv4) -> default_value("127.0.0.1"), \
             "ipv4 adress for tcp server")
		     ("port", po::value<size_t>(&a . server_port) -> required(), \
             "port to read from tcp server")
		     ("packetsize,p", po::value<size_t>(&a . tcp_packet_size) -> required(), \
             "size (in bytes/octets) of tcp packets to cut the payload into")
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
	
}

