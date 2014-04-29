#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	cout << "Start " << argv[1] << endl;

	VideoCapture vid(argv[1]);
	Mat img;

	if (!vid.isOpened())
	{
		cout << "Couldn't open Video." << endl;
		return 1;
	}

	for (;;)
	{
		vid >> img;
		if (!img.empty())
		{
			namedWindow("Window1", WINDOW_AUTOSIZE);
			namedWindow("Window2", WINDOW_AUTOSIZE);

			int x = img.cols / 2;
			int width = img.cols / 2;

			Mat img2 = img(Rect(x, 0, width, img.rows));
			img = img(Rect(0, 0, width, img.rows));

			imshow("Window1", img);
			imshow("Window2", img2);

			waitKey(1);
		}
		else
		{
			cout << "Couldn't read Image File." << endl;
			return 1;
		}
	}
}