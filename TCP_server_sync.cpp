//
// Created by j.bailleul on 09/08/2023.
// https://gwenael-dunand.developpez.com/tutoriels/cpp/boost/asio/
//
#define _WIN32_WINNT 0x0501 // Windows XP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <cstdlib>

using boost::asio::ip::tcp;

int main()
{
	size_t payload_size = 1024*1024*256 *sizeof(char);
	void* data = malloc(payload_size); // _aligned_malloc(payload_size, 1024) : pareil;
	if (data == nullptr) return 1;
	
	char* data_char = static_cast<char*>(data);
	for (size_t i = 0; i < payload_size; i++)
		data_char[i] = 'h';
	const char* toto = reinterpret_cast<const char*>( data );
	
	std::vector<char> vecdata(1024*1024*256*sizeof(char));
	for (auto &e : vecdata)
		e = 'g';
	
	
	// Création du service principal et du résolveur.
	boost::asio::io_service ios;
	
	// Création de l'acceptor avec le port d'écoute 7171 et une adresse quelconque de type IPv4 // (1)
	tcp::acceptor acceptor(ios, tcp::endpoint(tcp::v4(), 7171));
	
	std::string msg ("Bienvenue sur le serveur !"); // (2)
	// On attend la venue d'un client
	while (1)
		{
			// Création d'une socket
			tcp::socket socket(ios); // (3)
			
			// On accepte la connexion
			acceptor.accept(socket); // (4)
			std::cout << "Client recu ! " << std::endl;
			
			// On envoi un message de bienvenue
			// socket.send(boost::asio::buffer(msg)); // (5)
			// socket.send( boost::asio::buffer(vecdata));
			socket. send( boost::asio::buffer(toto, payload_size));
		}
		
	return 0;
}