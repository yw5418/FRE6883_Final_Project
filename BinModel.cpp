#include "BinModel.h"
#include <stdio.h>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"
#include <map>
#include <random>;
#include <algorithm>;
#include <iterator>;
using namespace std;
BinModel::BinModel(int N) {
	SetN(N);
	populateSymbolVector(symbolList, aday, cIWB1000MeetGroupFile);
	populateSymbolVector(symbolList, aday, cIWB1000HighGroupFile);
	populateSymbolVector(symbolList, aday, cIWB1000LowGroupFile);
	while (true) {
		if (N <= 30) {
			cout << "warning: illegal N, enter again: " << endl;
			cin >> N;
		}
		int min = stoi(getTimeinSeconds("2020-11-30T16:00:00"));
		int max = 0;
		for (int i = 0; i < aday.size(); ++i) {
			string day0 = get_Day0(aday[i]);
			int day0_sec = stoi(getTimeinSeconds(day0));
			if (day0_sec < min) {
				min = day0_sec;
			}
			if (day0_sec > max) {
				max = day0_sec;
			}
		}
		if (((max + N * 86400) > stoi(getTimeinSeconds("2020-11-30T16:00:00"))) || ((min - N * 86400) <= stoi(getTimeinSeconds("2020-01-03T16:00:00")))) {
			cout << "warning: illegal N, enter again: " << endl;
			cin >> N;
		}
		else {
			break;
		}
	}
	
}
void BinModel::SetN(int N_) {
	N = N_;
}
vector<int> BinModel::getrandom(vector<string> x) {
	int m = 0;
	int n = x.size();
	vector<int> v(n);
	for (int i = 0; i < n; i++) {
		v[i] = i;
	}
	srand((unsigned)time(NULL));
	random_shuffle(v.begin(), v.end());
	vector<int>::const_iterator first1 = v.begin();
	vector<int>::const_iterator last1 = v.begin() + 40;
	vector<int> cut1_vector(first1, last1);
	return cut1_vector;
}

void BinModel::populateSymbolVector(vector<string>& symbols, vector<string>& adays, const char* name) {
	ifstream fin;
	fin.open(name, ios::in);
	string line, day0, symbol0;
	vector<string> symbol;
	vector<string> aday;
	while (!fin.eof())
	{
		getline(fin, line);
		stringstream sin(line);
		getline(sin, symbol0, ',');
		getline(sin, day0);
		symbol.push_back(symbol0);
		aday.push_back(day0);
	}
	vector<int> rand_num = getrandom(symbol);
	vector<string> symbolList_40 = get_vec(rand_num, symbol);
	vector<string> aday_40 = get_vec(rand_num, aday);
	symbols.insert(symbols.end(), symbolList_40.begin(), symbolList_40.end());
	adays.insert(adays.end(), aday_40.begin(), aday_40.end());
}
vector<string> BinModel::get_vec(vector<int> id, vector<string> x) {
	vector<string> y;
	for (int i = 0; i < id.size(); ++i) {
		int idx = id[i];
		y.push_back(x[idx]);
	}
	return y;
}

string BinModel::getTimeinSeconds(string Time)
{
	std::tm t = { 0 };
	std::istringstream ssTime(Time);
	char time[100];
	memset(time, 0, 100);
	if (ssTime >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S"))
	{
		/*cout << std::put_time(&t, "%c %Z") << "\n"
			<< std::mktime(&t) << "\n";*/
		std::put_time(&t, "%c %Z");
		std::mktime(&t);
		sprintf(time, "%lld", mktime(&t));
		return string(time);
	}
	else
	{
		cout << "Parse failed\n";
		return "";
	}
}
string BinModel::get_Day0(string day) {
	string s1 = day.substr(6, 4);//year
	string s2 = day.substr(3, 2);//month
	string s3 = day.substr(0, 2);//day
	return s1 + "-" + s2 + "-" + s3 + "T16:00:00";
}
