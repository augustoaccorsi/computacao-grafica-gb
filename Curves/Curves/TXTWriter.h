#pragma once
#include "Includes.h"

#ifndef TXTWRITER_H
#define TXTWRITER_H

using namespace std;

class TXTWriter
{

public:
	ofstream TXTFile;

	TXTWriter() {	
	}

	void createTXTFile() {
		TXTFile.open("originalCurve.txt");
	}

	void addPoint(float x, float y, float z){	
		TXTFile << "v " << x << " " << z << " " << y << endl;
	}

	void closeTXTFile() {
		TXTFile.close();
	}
};

#endif