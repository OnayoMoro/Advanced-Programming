#include "Nodes.h"
#include <string>
#include <iostream>
using namespace std;

Nodes::~Nodes()
{
}

Nodes::Nodes(string& place, const int ref, const double lat, const double longitude) : m_g_place(place), m_g_ref(ref), m_g_lat(lat), m_g_longitude(longitude) {

}

const string& Nodes::g_place(void)const {
	return m_g_place;
}

int Nodes::g_ref(void)const {
	return m_g_ref;
}

double Nodes::g_lat(void)const {
	return m_g_lat;
}

double Nodes::g_longitude(void)const {
	return m_g_longitude;
}

void Nodes::add_arcs(Arcs * const arcs)
{
	m_g_arcs.push_back(arcs);
}

const vector<Arcs*>& Nodes::g_arcs()const
{
	return m_g_arcs;
}
