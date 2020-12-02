#include <stdio.h>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
std::string str;
#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"
#include <map>
#include <random>;
#include <algorithm>;
#include <iterator>;
#include "BinModel.h"
using namespace std;



/*write data to a file(used in building connection with yahoo.com)*/
int write_data(void* ptr, int size, int nmemb, FILE* stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return (int)written;
}

/*write data directly to memory(faster)*/
struct MemoryStruct {
	char* memory;
	size_t size;
};
void* myrealloc(void* ptr, size_t size)
{
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}
int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)data;
	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return (int)realsize;
}
void buildConnection(string url) {

}

int main(void)
{

	int N;
	cin >> N;

	int c = 0;
	while (c < 30) {
		BinModel Model(N);
		int page_num = 0;
		int datasize = 0;
		map<string, vector<double>> stock_price;
		map<string, vector<double>> IWB;
		string startTime = Model.getTimeinSeconds("2020-01-01T16:00:00");
		string endTime = Model.getTimeinSeconds("2020-11-30T16:00:00");
		/*sample 120 stocks(name)*/
		vector<string> symbolList = Model.getSymbol();
		/*sample 120 stocks(day0)*/
		vector<string> aday3_40 = Model.getDay0(); 
		
		vector<string>::iterator itr = symbolList.begin();
		vector<string>::iterator itr1 = aday3_40.begin();
		
		//store adj
		struct MemoryStruct data;
		data.memory = NULL;
		data.size = 0;
		//store IWB
		struct MemoryStruct data1;
		data1.memory = NULL;
		data1.size = 0;

		// file pointers to create file that store the data  
		FILE* fp1, * fp2;

		// name of files  
		const char outfilename[FILENAME_MAX] = "Output.txt";
		const char resultfilename[FILENAME_MAX] = "Results.txt";

		// declaration of an object CURL 
		CURL* handle;

		CURLcode result;

		// set up the program environment that libcurl needs 
		curl_global_init(CURL_GLOBAL_ALL);

		// curl_easy_init() returns a CURL easy handle 
		handle = curl_easy_init();

		// if everything's all right with the easy handle... 
		if (handle)
		{
			string sCookies, sCrumb;
			string sCookies1, sCrumb1;
			int IWB_flag = 0;
			fp1 = fopen(resultfilename, "w");
			while (true)
			{
				/*build connection with yahoo*/
				if (sCookies.length() == 0 || sCrumb.length() == 0)
				{
					fp2 = fopen(outfilename, "w");
					curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
					curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
					curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
					curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
					curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");

					curl_easy_setopt(handle, CURLOPT_ENCODING, "");
					curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
					curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp2);
					result = curl_easy_perform(handle);
					fclose(fp2);

					if (result != CURLE_OK)
					{
						// if errors have occurred, tell us what is wrong with result
						fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
						return 1;
					}

					curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
					curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)& data);

					// perform, then store the expected code in result
					result = curl_easy_perform(handle);

					if (result != CURLE_OK)
					{
						// if errors have occured, tell us what is wrong with result
						fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
						return 1;
					}

					char cKey[] = "CrumbStore\":{\"crumb\":\"";
					char* ptr1 = strstr(data.memory, cKey);
					char* ptr2 = ptr1 + strlen(cKey);
					char* ptr3 = strstr(ptr2, "\"}");
					if (ptr3 != NULL)
						* ptr3 = NULL;

					sCrumb = ptr2;

					fp2 = fopen("cookies.txt", "r");
					char cCookies[100];
					if (fp2) {
						while (fscanf(fp2, "%s", cCookies) != EOF);
						fclose(fp2);
					}
					else
						cerr << "cookies.txt does not exists" << endl;

					sCookies = cCookies;
					free(data.memory);
					data.memory = NULL;
					data.size = 0;
				}
				/*read IWB price to memory.data1 only once  */
				if (IWB_flag == 0) {
					string url = "https://query1.finance.yahoo.com/v7/finance/download/IWB?period1=1577912400&period2=1606770000&interval=1d&events=history&crumb=fR7nhq5OMzs";
					const char* cURL = url.c_str();
					const char* cookies = sCookies.c_str();
					curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);
					curl_easy_setopt(handle, CURLOPT_URL, cURL);
					if (result != CURLE_OK)
					{
						// if errors have occurred, tell us what is wrong with 'result'
						fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
						return 1;
					}
					curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
					curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)& data1);
					result = curl_easy_perform(handle);

					if (result != CURLE_OK)
					{
						// if errors have occurred, tell us what is wrong with result
						fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
						return 1;
					}
					IWB_flag += 1;
					
				}




				if (itr == symbolList.end())
					break;

				string day0 = Model.get_Day0(*itr1);
				int day0_sec = stoi(Model.getTimeinSeconds(day0));
				string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
				string symbol = *itr;

				string urlB = "?period1=";
				string urlC = "&period2=";
				string urlD = "&interval=1d&events=history&crumb=";
				string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
				const char* cURL = url.c_str();
				const char* cookies = sCookies.c_str();
				curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);
				curl_easy_setopt(handle, CURLOPT_URL, cURL);

				// Check for errors */
				if (result != CURLE_OK)
				{
					// if errors have occurred, tell us what is wrong with 'result'
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
					return 1;
				}
				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)& data);
				result = curl_easy_perform(handle);



				if (result != CURLE_OK)
				{
					// if errors have occurred, tell us what is wrong with result
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
					return 1;
				}
				/*for daj*/
				stringstream sData;
				sData.str(data.memory + datasize);
				datasize = data.size;
				string sValue, sDate;
				double dValue = 0;
				string line;
				getline(sData, line);
				vector<double> adj;
				while (getline(sData, line)) {
					sDate = line.substr(0, line.find_first_of(',')) + "T16:00:00";//2020-10-1
					int sDate_sec = stoi(Model.getTimeinSeconds(sDate));
					/*-N~N*/
					if ((sDate_sec >= (day0_sec - N * 86400)) && (sDate_sec <= (day0_sec + N * 86400))) {
						line.erase(line.find_last_of(','));
						sValue = line.substr(line.find_last_of(',') + 1);
						dValue = strtod(sValue.c_str(), NULL);//adj price
						adj.push_back(dValue);
					}

				}


				/*for IWB*/
				stringstream sData1;
				sData1.str(data1.memory);
				string sValue1, sDate1;
				double dValue1 = 0;
				string line1;
				getline(sData1, line1);
				vector<double> adj1;

				while (getline(sData1, line1)) {
					sDate1 = line1.substr(0, line1.find_first_of(',')) + "T16:00:00";//2020-10-1
					int sDate_sec = stoi(Model.getTimeinSeconds(sDate1));
					/*-N~N*/
					if ((sDate_sec >= (day0_sec - N * 86400)) && (sDate_sec <= (day0_sec + N * 86400))) {
						line1.erase(line1.find_last_of(','));
						sValue1 = line1.substr(line1.find_last_of(',') + 1);
						dValue1 = strtod(sValue1.c_str(), NULL);//adj price
						adj1.push_back(dValue1);
					}

				}

				
				page_num += adj.size();



				
				stock_price[symbol] = adj;
				IWB[symbol] = adj1;
				adj.clear();
				itr++;
				itr1++;
				
			}


			cout << c << endl;
			/*fclose(fp1);*/
			free(data.memory);
			free(data1.memory);
			data.size = 0;
			data1.size = 0;
		}
		else
		{
			fprintf(stderr, "Curl init failed!\n");
			return 1;
		}

		// cleanup since you've used curl_easy_init  
		curl_easy_cleanup(handle);

		// release resources acquired by curl_global_init() 
		curl_global_cleanup();
		c += 1;
	}
	return 0;
}