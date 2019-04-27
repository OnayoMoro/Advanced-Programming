#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "Arcs.h"



using namespace std;


class Navigation
{
	std::vector<Arcs*> arc;
	std::ofstream _outFile;

	// Add your code here

public:

	Navigation();
	~Navigation();

	bool FindRoute(const int start, const int end, const std::string& tr, const std::string& R);
	bool ConstructRoute(vector<int>& visited, vector<int>& clonerefs, int start, int end);
	bool ConstructShortestRoute(vector<int>& visited, vector<int>& clonerefs, const int start, const int end);
	bool PrintRoute(vector<int>& MyRoute);
	bool Route(vector<int>& currentrefs, int start, int end, const std::string& tr, const std::string& R);
	const bool BuildNetwork(const std::string& fileNamePlaces, const std::string& fileNameLinks)const;
	int Transport(const std::string& tr, int ref1, int ref2);
	bool ProcessCommand(const std::string& commandString);
	bool MaxDist();
	bool MaxLink();
	bool FindDist(int start, int end);
	bool FindNeighbour(int ref);
	bool Check(const std::string& transport, const std::vector<int>& references);
	const inline double Distance(const double x1, const double y1, const double x2, const double y2)const {
		double dist; return dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	}
	const inline std::vector<string> TransType(const std::string& tr)const {
		vector<string> method;
		if (tr == "Rail") {
			method.push_back("Rail");
		}
		else if (tr == "Ship") {
			method.push_back("Ship");
		}
		else if (tr == "Bus") {
			method.push_back("Bus");
			method.push_back("Ship");
		}
		else if (tr == "Car") {
			method.push_back("Car");
			method.push_back("Bus");
			method.push_back("Ship");
		}
		else if (tr == "Bike") {
			method.push_back("Car");
			method.push_back("Bus");
			method.push_back("Ship");
			method.push_back("Rail");
			method.push_back("Bike");
		}
		else if (tr == "Foot") {
			method.push_back("Car");
			method.push_back("Bus");
			method.push_back("Ship");
			method.push_back("Rail");
			method.push_back("Bike");
			method.push_back("Foot");
		}
		return method;
	}
};