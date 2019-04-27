#pragma once
#include <string>
#include "Arcs.h"
#include <vector>
using namespace std;

class Arcs;

class Nodes
{
public:
	Nodes(string& place, int ref, double lat, double longitude);
	~Nodes();
	Nodes(const Nodes&);
	Nodes& operator=(const Nodes&);

	const string& g_place(void)const;
	int g_ref(void)const;
	double g_lat(void)const;
	double g_longitude(void)const;
	void add_arcs(Arcs* arcs);
	const vector<Arcs*>& g_arcs()const;

private:
	vector<Arcs*> m_g_arcs;
	string const m_g_place;
	int m_g_ref;
	double m_g_lat;
	double m_g_longitude;
};

