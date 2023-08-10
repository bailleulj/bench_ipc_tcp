//
// Created by j.bailleul on 09/08/2023 from:
// https://gwenael-dunand.developpez.com/tutoriels/cpp/boost/asio/
//

#define _WIN32_WINNT 0x0501 // Windows XP

#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <cstdlib>


/// structure remplie par ligne de commande
struct progArgs
{
  std::string server_ipv4; // ipv4 adress string for server
  size_t      server_port; // port number
  size_t      sm_size_bytes; // payload size in octets / Bytes
  size_t      sm_size_mega;  // same in Mo / Mb : ignoré par la suite
  char        value; // valeur unique de tous les élts de la payload
};

static void usage(int argc, char **argv, progArgs &a);

const char *srv_ipv4_g; /// global contenant


using boost::asio::ip::tcp;

int main(int argc, char**argv)
{
	// usage & argument parsing
	progArgs args;
	usage(argc, argv, args);
	
	
	
	size_t payload_size = args.sm_size_bytes; //1024 * 1024 * 256 * sizeof(char);
	void *data = malloc(payload_size); // _aligned_malloc(payload_size, 1024) : pareil;
	if ( data == nullptr ) return 1;
	
	char *data_char = static_cast<char *>(data);
	for (size_t i = 0; i < payload_size; i ++)
		data_char[i] = args.value;
	const char *payload_buffer = reinterpret_cast<const char *>( data );
	
	std::vector<char> vecdata(args.sm_size_bytes);
	for (auto         &e: vecdata)
		e = args.value;
	
	
	// Création du service principal et du résolveur.
	boost::asio::io_service ios;
	
	// Création de l'acceptor avec le port d'écoute 7171 et une adresse quelconque de type IPv4 // (1)
	tcp::acceptor acceptor(ios, tcp::endpoint(tcp::v4(), args.server_port));
	
	std::string msg("Bienvenue sur le serveur !"); // (2)
	// On attend la venue d'un client
	while (1)
		{
			// Création d'une socket
			tcp::socket socket(ios); // (3)
			
			// On accepte la connexion
			acceptor . accept(socket); // (4)
			std::cout << "Client recu ! " << std::endl;
			
			// On envoie un message de bienvenue
			// socket.send(boost::asio::buffer(msg)); // (5)
			// socket.send( boost::asio::buffer(vecdata));
			socket . send(boost::asio::buffer(payload_buffer, payload_size));
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
		     ("srvip,i", po::value<std::string>(&a . server_ipv4) -> default_value("127.0.0.1"), \
             "ipv4 adress for tcp server")
		     ("port,p", po::value<size_t>(&a.server_port)->required(), \
			 "port to read from tcp server")
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

