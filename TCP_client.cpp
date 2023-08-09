//
// Created by j.bailleul on 09/08/2023.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/chrono.hpp>

int main()
{
	boost::chrono::high_resolution_clock::time_point t1, t2, t3;
	
	
	// Création du service principal et du résolveur.
	boost::asio::io_service ios;
	
	using namespace boost::asio;
	using ip::tcp;
	
	// On veut se connecter sur la machine locale, port 7171
	tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 7171);
	
	// On crée une socket // (1)
	tcp::socket socket(ios);
	
	// Tentative de connexion, bloquante // (2)
	socket.connect(endpoint);
	
	// Création du buffer de réception // (3)
	const size_t buffer_size = 1024*256;
	boost::array<char, buffer_size> buf;
	
	
	// on découpe le message en série de buffers de buffer_size
	t1 = boost::chrono::high_resolution_clock::now();
	size_t total_len = 0;
	while (1)
		{
			boost::system::error_code error;
			// Réception des données, len = nombre d'octets reçus // (4)
			size_t len = socket.read_some(boost::asio::buffer(buf), error);
			total_len += len;
			
			if (error == boost::asio::error::eof) // (5)
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
	t2 = boost::chrono::high_resolution_clock::now();
	auto duration1 = (boost::chrono::duration_cast<boost::chrono::milliseconds>(t2-t1));
	std::cout << "\nclient: durations (ms):" << duration1.count();
	
	float payload_mb = static_cast<float>(total_len) / (1024 * 1024);
	float bandwidth = payload_mb * (1000.0f / duration1.count());
	
	std::cout << "\nclient: bandwidth (Mo/s): " << bandwidth << " (packet size: " << buffer_size << ")";
	
	return 0;
}