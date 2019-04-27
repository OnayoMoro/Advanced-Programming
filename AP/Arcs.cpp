#include "Arcs.h"
#include "Nodes.h"
using namespace std;

Arcs::~Arcs()
{
}

Arcs::Arcs(const int startref, const int endref, string& transport_method) : m_g_startref(startref), m_g_endref(endref), m_g_transport_method(transport_method), m_g_destination(){

}

int Arcs::g_startref(void)const {
	return m_g_startref;
}

int Arcs::g_endref(void)const {
	return m_g_endref;
}

const string& Arcs::g_transport_method(void)const {
	return m_g_transport_method;
}

void Arcs::s_destination(Nodes* const n) 
{
	m_g_destination = n;
}

