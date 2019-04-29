#include "Navigation.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "Nodes.h"
#include "Arcs.h"

using namespace std;

vector<Nodes*> nodes;
vector<Arcs*> arcs;

// Converts latitude/longitude into eastings/northings
extern void LLtoUTM(const double Lat, const double Long, double &UTMNorthing, double &UTMEasting);

Navigation::Navigation() : _outFile("Output.txt")
{
}

Navigation::~Navigation()
{
}

bool Navigation::ProcessCommand(const string& commandString)
{
	istringstream inString(commandString);
	string command;
	inString >> command;

	if (command == "MaxDist") {
		return MaxDist();
	}
	else if (command == "MaxLink") {
		return MaxLink();
	}
	else if (command == "FindDist") {
		int start, end;
		inString >> start;
		inString >> end;
		return FindDist(start, end);
	}
	else if (command == "FindNeighbour") {
		int ref;
		inString >> ref;
		return FindNeighbour(ref);
	}
	else if (command == "Check") {
		int ref = 0;
		string tr;
		inString >> tr;
		vector<int> references;
		while (inString >> ref)
			references.push_back(ref);
		Check(tr, references);
		return true;
	}
	else if (command == "FindRoute") {
		int start = 0, end = 0; string tr; const string& FR = "FR";
		inString >> tr;
		inString >> start;
		inString >> end;
		_outFile << "FindRoute " << tr << " " << start << " " << end;
		return FindRoute(start, end, tr, FR);
	}
	else if (command == "FindShortestRoute") {
		int start = 0, end = 0; string tr; const string& FSR = "FSR";
		inString >> tr;
		inString >> start;
		inString >> end;
		_outFile << "FindShortestRoute " << tr << " " << start << " " << end;
		return FindRoute(start, end, tr, FSR);
	}return false;
}

const bool Navigation::BuildNetwork(const string &fileNamePlaces, const string &fileNameLinks)const
{
	fstream finPlaces(fileNamePlaces), finLinks(fileNameLinks);
	char line[255];
	double latitude, longitude;
	int startRef, endRef, ref;

	if (finPlaces.fail() || finLinks.fail()) {
		return false;
	}
	else
	{
		int i = 0; double Northings, Eastings;
		while (finPlaces.good())
		{
			finPlaces.getline(line, 255, ',');
			string placeName = string(line);

			finPlaces.getline(line, 255, ',');
			istringstream sin(line);
			sin >> ref;

			finPlaces.getline(line, 255, ',');
			istringstream sin1(line);
			sin1 >> latitude;

			finPlaces.getline(line, 255, '\n');
			istringstream sin2(line);
			sin2 >> longitude;

			LLtoUTM(latitude, longitude, Northings, Eastings);

			Nodes* const n = new Nodes(placeName, ref, Northings, Eastings);
			nodes.push_back(n);
		}
		while (finLinks.good())
		{
			finLinks.getline(line, 255, ',');
			istringstream sin(line);
			sin >> startRef;

			finLinks.getline(line, 255, ',');
			istringstream sin1(line);
			sin1 >> endRef;

			finLinks.getline(line, 255, '\n');
			string transport = string(line);

			Arcs* const a = new Arcs(startRef, endRef, transport);
			arcs.push_back(a);

			for (Nodes* const n : nodes) {
				if (n->g_ref() == startRef) {
					arcs[i]->s_destination(n);
					Arcs* const ar = new Arcs(startRef, endRef, transport);
					n->add_arcs(ar);
					for (Nodes* const no : nodes)
						if (endRef == no->g_ref())
							ar->s_destination(no);
				}
				else if (n->g_ref() == endRef) {
					Arcs* const ar = new Arcs(endRef, startRef, transport);
					n->add_arcs(ar);
					for (Nodes* const no : nodes)
						if (startRef == no->g_ref())
							ar->s_destination(no);
				}
			}i++;
		}
	}return true;
}

bool Navigation::MaxDist() {
	string start, end; double dist, max = 0;

	for (int i = 0; i < nodes.size(); i++)
	{
		const double x1 = nodes[i]->g_lat();
		const double y1 = nodes[i]->g_longitude();
		for (int p = i + 1; p < nodes.size(); p++)
		{
			const double x2 = nodes[p]->g_lat();
			const double y2 = nodes[p]->g_longitude();
			dist = Distance(x1, y1, x2, y2);
			if (dist / 1000 > max)
			{
				start = nodes[i]->g_place();
				end = nodes[p]->g_place();
				max = dist / 1000;
			}
		}
	}
	_outFile << "MaxDist \n" << start << "," << end << "," << max << "\n" << endl; return true;
}

bool Navigation::MaxLink() {
	int maxstart = 0, maxend = 0, bothrefs = 0;
	double x1, y1, x2, y2, max = 0;

	for (Arcs* const a : arcs)
	{
		for (Nodes* const n : nodes)
		{
			if (a->g_startref() == n->g_ref()) {
				bothrefs += 1;
				x1 = n->g_lat();
				y1 = n->g_longitude();
			}
			else if (a->g_endref() == n->g_ref()) {
				bothrefs += 1;
				x2 = n->g_lat();
				y2 = n->g_longitude();
			}
			if (bothrefs == 2) {
				bothrefs = 0;
				const double dist = Distance(x1, y1, x2, y2);
				if (dist / 1000 > max)
				{
					max = dist / 1000;
					maxstart = a->g_startref();
					maxend = a->g_endref();
				}
				break;
			}
		}
	}_outFile << "MaxLink \n" << maxstart << "," << maxend << "," << max << "\n" << endl; return true;
}

bool Navigation::FindDist(const int start, const int end) {
	string startplace, endplace;
	double x1, y1, x2, y2; int bothrefs = 0;

	for (Nodes* const n : nodes)
	{
		if (start == n->g_ref()) {
			bothrefs += 1;
			x1 = n->g_lat();
			y1 = n->g_longitude();
			startplace = n->g_place();
		}
		else if (end == n->g_ref()) {
			bothrefs += 1;
			x2 = n->g_lat();
			y2 = n->g_longitude();
			endplace = n->g_place();
		}
		if (bothrefs == 2) {
			const double dist = Distance(x1, y1, x2, y2) / 1000;
			_outFile << "FindDist " << start << " " << end << "\n" << startplace << "," << endplace << "," << dist << "\n" << endl;
			return true;
		}
	}
	return true;
}

bool Navigation::FindNeighbour(const int ref) {
	string neighb;
	_outFile << "FindNeighbour " << ref << endl;
	for (Nodes* const n : nodes) {
		if (ref == n->g_ref()) {
			vector<Arcs*> const arcs = n->g_arcs();
			for (Arcs* const a : arcs) {
				neighb = neighb + to_string(a->g_endref()) + "\n";
			}
			_outFile << neighb << endl; return true;
		}
	}return true;
}

bool Navigation::Check(const std::string& transport, const std::vector<int>& references) {
	string refs; int passfail = 0; int ref1, ref2;
	for (const int r : references)
		refs = refs + to_string(r) + " ";

	_outFile << "Check " << transport << " " << refs << endl;

	for (int r = 0; r < references.size() - 1; r++)
	{
		ref1 = references[r]; ref2 = references[r + 1];
		passfail = Transport(transport, ref1, ref2);
		if (passfail == 1)
		{
			_outFile << ref1 << "," << ref2 << "," << "FAIL" << "\n" << endl; return true;
		}
	}
	_outFile << endl; return true;
}

int Navigation::Transport(const std::string& transport, const int ref1, const int ref2) {
	for (Arcs* const a : arcs)
	{
		if ((ref1 == a->g_startref() || ref1 == a->g_endref()) && (ref2 == a->g_startref() || ref2 == a->g_endref()))
		{
			vector<string> method; method = TransType(transport);
			int c = 0;
			for (const string m : method)
			{
				if (a->g_transport_method() == m)
				{
					_outFile << ref1 << "," << ref2 << "," << "PASS" << "\n"; c++;
				}
			}
			if (c == 0)
			{
				return 1; break;
			}
			else
				return 0;
		}
	}return 0;
}

bool Navigation::FindRoute(const int start, const int end, const std::string& tr, const std::string& R)
{
	vector<string> transport; transport = TransType(tr);
	vector<int> currentrefs;
	for (int n = 0; n < nodes.size(); n++)
	{
		if (nodes[n]->g_ref() == start)
		{
			vector<Arcs*> myarcs = nodes[n]->g_arcs();
			for (int a = 0; a < myarcs.size(); a++)
			{
				if (end == myarcs[a]->g_endref() && (count(transport.begin(), transport.end(), myarcs[a]->g_transport_method())))
				{
					_outFile << "\n" << start << "\n" << end << "\n" << endl;
					return true;
				}
				else if (count(transport.begin(), transport.end(), myarcs[a]->g_transport_method()))
				{
					currentrefs.push_back(myarcs[a]->g_endref());
				}
			}
			if (myarcs.size() != 0)
			{
				Route(currentrefs, start, end, tr, R);
				return true;
			}
			else if (myarcs.size() == 0)
			{
				_outFile << "\n" << "FAIL" << "\n" << endl;
				return true;
			}

		}
	}
	return true;
}

bool Navigation::Route(vector<int>& currentrefs, const int start, const int end, const string& tr, const string& R)
{
	vector<int> clonerefs = currentrefs;
	vector<string> transport; transport = TransType(tr);
	vector<int> visited; visited.push_back(start);
	for (int c = 0; c < currentrefs.size(); c++)
	{
		for (int n = 0; n < nodes.size(); n++)
		{
			if (currentrefs.size() != 0 && nodes[n]->g_ref() == currentrefs[c])
			{
				if (!count(visited.begin(), visited.end(), currentrefs[c]))
				{
					visited.push_back(currentrefs[c]);
				}
				vector<Arcs*> myarcs = nodes[n]->g_arcs();
				for (int a = 0; a < myarcs.size(); a++)
				{
					if ((count(transport.begin(), transport.end(), myarcs[a]->g_transport_method())))
					{
						if (end == myarcs[a]->g_endref())
						{
							if (R == "FR")
							{
								visited.push_back(myarcs[a]->g_startref());
								ConstructRoute(visited, clonerefs, transport, start, end);
								return true;
							}
							else if (R == "FSR")
							{
								visited.push_back(myarcs[a]->g_startref());
								ConstructShortestRoute(visited, clonerefs, transport, start, end);
								return true;
							}
						}
						else if (!count(visited.begin(), visited.end(), myarcs[a]->g_endref()))
						{
							currentrefs.push_back(myarcs[a]->g_endref());
						}
					}
				}
				currentrefs.erase(currentrefs.begin());
				n = -1; c = 0;
			}
		}
	}
	_outFile << "\n" << "FAIL" << "\n" << endl;
	return true;
}

bool Navigation::ConstructShortestRoute(vector<int>& visited, vector<int>& clonerefs, vector<string>& transport, const int start, const int end)
{
	reverse(visited.begin(), visited.end()); int ref;
	vector<int> MyRoute; MyRoute.push_back(end);

	for (int r = 0; r < MyRoute.size(); r++)
	{
		for (int v = 0; v < visited.size(); v++)
		{
			for (int m = 0; m < arcs.size(); m++)
			{
				for (const int c : clonerefs)
				{
					if ((MyRoute.back() == arcs[m]->g_startref() && c == arcs[m]->g_endref()) && (count(transport.begin(), transport.end(), arcs[m]->g_transport_method())))
					{
						MyRoute.push_back(c); MyRoute.push_back(start); PrintRoute(MyRoute);
						return true;
					}
					else if ((MyRoute.back() == arcs[m]->g_endref() && c == arcs[m]->g_startref()) && (count(transport.begin(), transport.end(), arcs[m]->g_transport_method())))
					{
						MyRoute.push_back(c); MyRoute.push_back(start); PrintRoute(MyRoute);
						return true;
					}
				}
				if ((MyRoute[r] == arcs[m]->g_startref() && visited[v] == arcs[m]->g_endref()) && (count(transport.begin(), transport.end(), arcs[m]->g_transport_method())))
				{
					ref = visited[v];
					visited.erase(visited.begin() + v);
				}
				else if ((MyRoute[r] == arcs[m]->g_endref() && visited[v] == arcs[m]->g_startref()) && (count(transport.begin(), transport.end(), arcs[m]->g_transport_method())))
				{
					ref = visited[v];
					visited.erase(visited.begin() + v);
				}
			}
		}
		MyRoute.push_back(ref);
	}
	_outFile << "\n" << "FAIL" << "\n" << endl;
	return true;
}

bool Navigation::ConstructRoute(vector<int>& visited, vector<int>& clonerefs, vector<string>& transport, const int start, const int end)
{
	reverse(visited.begin(), visited.end());
	vector<int> MyRoute; MyRoute.push_back(end);
	for (int v = 0; v < visited.size(); v++)
	{
		for (int m = 0; m < arcs.size(); m++)
		{
			for (const int c : clonerefs)
			{
				if ((MyRoute.back() == arcs[m]->g_startref() && c == arcs[m]->g_endref()))
				{
					MyRoute.push_back(c); MyRoute.push_back(start);
					PrintRoute(MyRoute);
					return true;
				}
				else if ((MyRoute.back() == arcs[m]->g_endref() && c == arcs[m]->g_startref()))
				{
					MyRoute.push_back(c); MyRoute.push_back(start);
					PrintRoute(MyRoute);
					return true;
				}
			}
			if ((MyRoute.back() == arcs[m]->g_startref() && visited[v + 1] == arcs[m]->g_endref()) && (count(transport.begin(), transport.end(), arcs[m]->g_transport_method())))
			{
				MyRoute.push_back(visited[v + 1]);
				visited.erase(visited.begin() + v);
			}
			else if ((MyRoute.back() == arcs[m]->g_endref() && visited[v + 1] == arcs[m]->g_startref()) && (count(transport.begin(), transport.end(), arcs[m]->g_transport_method())))
			{
				MyRoute.push_back(visited[v + 1]);
				visited.erase(visited.begin() + v);
			}
		}
	}
	_outFile << "\n" << "FAIL" << "\n" << endl;
	return true;
}

bool Navigation::PrintRoute(vector<int>& MyRoute)
{
	string route; reverse(MyRoute.begin(), MyRoute.end());
	for (const int mr : MyRoute)
	{
		route = route + "\n" + to_string(mr);
	}
	_outFile << route << "\n" << endl;
	return true;
}