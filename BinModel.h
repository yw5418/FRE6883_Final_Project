#define BinModel_h
#include <vector>
#include <string> 
using namespace std;
class BinModel {
private:
	const char* cIWB1000MeetGroupFile = "MeetGroup.csv";
	const char* cIWB1000HighGroupFile = "HighGroup.csv";
	const char* cIWB1000LowGroupFile = "LowGroup.csv";
	vector<string> symbolList;
	vector<string> aday;
	int N;
public:
	/*operate all the functions*/
	BinModel(int N);
	/*sample 40 stocks from each group. symbols:stock name, adays: day0(original days)*/
	void populateSymbolVector(vector<string>& symbols, vector<string>& adays, const char* name);
	vector<int> getrandom(vector<string> x);//generate a vector with the same size of symbols and shuffle it
	vector<string> get_vec(vector<int> id, vector<string> x);//sample a vector(size:40) from x(a group)
	/*user input*/
	void SetN(int N_);
	
	vector<string> getSymbol() {
		return symbolList;
	};
	vector<string> getDay0() {
		return aday;
	};
	
	string getTimeinSeconds(string Time);//convert standard from of days to seconds
	string get_Day0(string day);//convert days to standard form
	int getN_() {
		return N;
	}

};