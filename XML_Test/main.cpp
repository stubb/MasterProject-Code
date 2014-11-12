#include <iostream>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

int main()
{
	XMLDocument xml;
	xml.LoadFile("test.xml");

	const char* timestamp = xml.FirstChildElement("package")->FirstChildElement("location")->FirstChildElement("latitude")->GetText();
	cout << timestamp << endl;
}