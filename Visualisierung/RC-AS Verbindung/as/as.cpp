#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define DEBUG 0

#if defined _WIN32
	#include "dirent.h"
	#include <WinSock2.h>
	#pragma comment(lib, "libws2_32.a")
#else
	#include <dirent.h>
	#include <sys/socket.h>
	#include <sys/types.h>
#endif

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//cout << "Start " << argv[1] << endl;
	int MAX_CONNECTION_TRIES = 10;
	int IMAGE_CHUNKS = 10;

	VideoCapture vid(argv[1]);
	Mat img;

	if (!vid.isOpened())
	{
		cout << "Couldn't open Video." << endl;
		return 1;
	}

	SOCKET s;
	int rc;	//Rueckgabewert
	struct sockaddr_in addr; //IPv4 Adresse

	//WIN spezifisch ANFANG
	{
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2,2), &wsaData);
	}
	//WIN spezifisch ENDE

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(11111);

	int connection_tries = 0;
	do
	{
		connection_tries++;
		rc = connect(s, (struct sockaddr*)&addr, sizeof(addr));
		waitKey(100);
	} while (rc < 0 && connection_tries <= MAX_CONNECTION_TRIES) ;

	if (rc < 0)
	{
		cout << "Couldn't connect to Rendering Client. Max tries exceeded." << endl;
		return 1;
	}
	else
	{
		int meta_data[3] = {IMAGE_CHUNKS, vid.get(CV_CAP_PROP_FRAME_WIDTH), vid.get(CV_CAP_PROP_FRAME_HEIGHT)};
		rc = send(s, (char*)meta_data, 3 * sizeof(int), 0);
		for (;;)
		{
			vid >> img;
			if (!img.empty())
			{
				int x = img.cols / 2;
				int y = img.rows / 2;
				int width = img.cols / 2;
				//img = img(Rect(0, 0, width, img.rows)).clone();
				//img = Mat(img, Range(0, width));
				//resize(img, img, Size(), 0.5, 0.5, INTER_NEAREST);
				img = img.reshape(0,1);
				for (int i = 0; i < IMAGE_CHUNKS; i++)
				{
					#if DEBUG
						cout << "Position " << i * (img.total() * img.elemSize() / IMAGE_CHUNKS) << " with Length " << img.total() * img.elemSize() / IMAGE_CHUNKS << endl;
					#endif
					rc = send(s, (char*) img.data + i * img.total() * img.elemSize() / IMAGE_CHUNKS, img.total() * img.elemSize() / IMAGE_CHUNKS, 0);
					if (rc < 0)
					{
						printf("send failed with code %i %i...\nexit\n", rc, errno);
						return 1;
					}
				}
				waitKey(42);
			}
			else
			{
				cout << "Couldn't read Image File." << endl;
				return 1;
			}
		}
	}

	WSACleanup;
}