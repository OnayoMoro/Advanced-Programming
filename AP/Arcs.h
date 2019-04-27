#pragma once
#include <string>
#include "Nodes.h"
using namespace std;

class Nodes;

class Arcs
{
public:
	Arcs(int startRefNumber, int endRefNumber, string& transport);
	Arcs(const Arcs&);
	Arcs& operator=(const Arcs&);
	~Arcs();

	int g_startref(void)const;
	int g_endref(void)const;
	const string& g_transport_method(void)const;
	void s_destination(Nodes* const n);

private:
	int m_g_startref;
	int m_g_endref;
	string const m_g_transport_method;
	Nodes* m_g_destination;

};

