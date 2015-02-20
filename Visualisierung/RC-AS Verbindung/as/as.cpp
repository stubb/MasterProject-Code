/*	Debug Mode.	*/
#define DEBUG 1

/*	Picture Settings.	*/
#define NUMBER_OF_COLOR_CHANNELS 3

/*	Maximum Connection tries for Connection to RC.	*/
#define MAX_CONNECTIONS_TRIES 2

/*	User Functions.	*/
#include "Rendering_Client.h"
#include "MonkeyMediaProcessor.h"
#include "as.h"

/*void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
	tinyxml2::XMLDocument document;
	document.Parse(msg->get_payload().c_str());
	
	//TODO alles :D
	//get meta data
	int image_dimensions[2] = {0};
	document.FirstChildElement( "package" )->FirstChildElement( "width" )->QueryIntText( &image_dimensions[0] );
	document.FirstChildElement( "package" )->FirstChildElement( "height" )->QueryIntText( &image_dimensions[1] );
	std::cout << image_dimensions[0] << " " << image_dimensions[1] << std::endl;
	
	// send meta data
	rc = SDLNet_TCP_Send (tcpsock, image_dimensions, sizeof(int) * 2);
	if(rc < (signed) sizeof(int) * 2) 
	{
		printf("Sending metadata failed with code %i.\n", rc);
	}
	else
	{
		XMLElement* data = document.FirstChildElement( "package" )->FirstChildElement( "data" );
		
		char pixel_data[image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS];
		char * pEnd = (char *) data->GetText();

		for (int i = 0; i < image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS; ++i)
		{		
			pixel_data[i] = strtol(pEnd, &pEnd, 10);
		}
		
#if DEBUG
		for (int i = 0; i < 25; ++i)
		{
			std::cout << pixel_data[i] << std::endl;
		}
#endif
			
			
		for (int i = 0; i < NUMBER_OF_RENDERING_CLIENTS; ++i)
		{
			rc = SDLNet_TCP_Send (tcpsock, pixel_data, (image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS) + 1);
			if(rc < image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS) 
			{
				printf("Sending data failed with code %i.\n", rc);
			}
		}
	}
}*/

int main(int argc, char** argv)
{
	int NUMBER_OF_RENDERING_CLIENTS = 1;
	vector<Rendering_Client> *rendering_clients = new vector<Rendering_Client>();

	int return_code = 0;
	int libws_return_code = 0;

	struct libwebsocket_context *context;
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);
	info.port = 9000;
	info.gid = -1;
	info.uid = -1;
	info.protocols = protocols;

	vector<char*> *host_ips = new vector<char*>();
	host_ips->push_back("127.0.0.1");

	int host_port = 2000;

	while (libws_return_code >= 0) {
		libws_return_code = getopt_long(argc, argv, "ci:hsp:d:", options, NULL);
		if (libws_return_code < 0)
			continue;
		switch (libws_return_code) {
		case 'p':
			info.port = atoi(optarg);
			break;
		case 'y':
			host_port = atoi(optarg);
			break;
		case 'h':
			cerr << "Usage: Applicationserver [--port=<p>]\n" << endl;
			exit(1);
		}
	}

	#if DEBUG
		for (unsigned int i = 0; i < host_ips->size(); ++i)
			cout << "Host IP: " << host_ips->at(i) << endl;
		cout << "Host Port: " << host_port << endl;
	#endif

	return_code = init_rc_connections(rendering_clients, host_ips, host_port);

	if (return_code == 0 && rendering_clients->size() > 0)
	{
		libws_return_code = 0;
		context = libwebsocket_create_context(&info);
		if (context == NULL)
		{
			cerr << "libwebsocket init failed" << endl;
			return EXIT_FAILURE;
		}
		else
		{
			mmp = new MonkeyMediaProcessor(NUMBER_OF_RENDERING_CLIENTS);
			while (!libws_return_code)
			{
				libws_return_code = libwebsocket_service(context, 100);
			}
		}
	}
	else
	{
		cerr << "Couldn't establish Connections. Shutting down " << argv[0] << "." << endl;
		return(return_code);
	}

	libwebsocket_context_destroy(context);
	return(EXIT_SUCCESS);
}
