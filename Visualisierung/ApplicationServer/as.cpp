/*	Debug Mode.	*/
#define DEBUG 1

/*	Picture Settings.	*/
#define NUMBER_OF_COLOR_CHANNELS 3

/*	User Functions.	*/
#include "as.h"
#include "NetworkHandler.h"

// global var definitions
MonkeyMediaProcessor *mmp = NULL;

int main(int argc, char** argv)
{
	int NUMBER_OF_RENDERING_CLIENTS = 1;
	vector<Rendering_Client> *rendering_clients = new vector<Rendering_Client>();

	int return_code = 0;
	int libws_return_code = 0;

	vector<char*> *host_ips = new vector<char*>();
	host_ips->push_back("127.0.0.1");

	int host_port = 2000;
	int port = 9000;

	while (libws_return_code >= 0) {
		libws_return_code = getopt_long(argc, argv, "ci:hsp:d:", options, NULL);
		if (libws_return_code < 0)
			continue;
		switch (libws_return_code) {
		case 'p':
			port = atoi(optarg);
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
		mmp = new MonkeyMediaProcessor(NUMBER_OF_RENDERING_CLIENTS);
		NetworkHandler* nwh = new NetworkHandler(port);
		nwh->run();
		nwh->destroy();
	}
	else
	{
		cerr << "Couldn't establish Connections. Shutting down " << argv[0] << "." << endl;
		return(return_code);
	}
	return(EXIT_SUCCESS);
}
