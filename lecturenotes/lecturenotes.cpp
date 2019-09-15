// lecturenotes.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <curl/curl.h>
#include <string.h>
#include <sstream>
//#include <errno.h>
#include <cpr/cpr.h>
#include <regex>
//#include <iterator>
#include "ProgressBar.hpp"
#include <boost/filesystem.hpp>


using namespace std;

// std::to_string not working
namespace patch
{
	template < typename T > string to_string(const T& n)
	{
		ostringstream stm;
		stm << n;
		return stm.str();
	}
}

string match(string str,regex reg){
	
		//smatch matches;
		//regex_search(str, matches, reg);

	   //initializing iterators
		sregex_iterator current_match(str.begin(), str.end(), reg);
		sregex_iterator last_match;
		sregex_iterator temp_iter;

		while (current_match != last_match) {
			//cout << current_match->str(1);
            temp_iter = current_match;
			current_match++;

		}
		//cout << temp_iter->str(1)<< endl;
		return temp_iter->str(1);
	
}
int dl_jpg(string url,string file_name) {
	CURL* curl = curl_easy_init();
	FILE* fp;
	CURLcode result;
	string full_path = "./images/" + file_name + ".jpg";
	errno_t err;
	
	
	if ((err = fopen_s(&fp, full_path.c_str(), "wb")) != 0)
	{
		cout <<"File was not opened"<< endl;
		return 0;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
	
	result = curl_easy_perform(curl);
	//cout << file_name << ' ';
	//if (result == CURLE_OK)
	//{
	//	cout << "Downloaded" << endl;
	//}
	if(result != CURLE_OK)
	{
		cout << "Failed Reason : " << curl_easy_strerror( result ) << endl ;
	}
	
	fclose(fp);
	curl_easy_cleanup(curl);
}

int main(int argc,char **argv)
{   
	if (argc != 2)
	{
		cout << "usage: lecturenotes <url>";
		return 0;
	}

	if (!boost::filesystem::is_directory("./images")) {
		cout << "creating images subdirectory" << endl;
		boost::filesystem::create_directory("./images");

	}
	//regex
	regex reg_page_url(R"((/uploads/upload/note/\w+/\w+/[a-zA-Z0-9-]+\.(jpeg|jpg)))");
	regex reg_total_pages(R"("numberOfPages":\s(\d+),)");
	
	//Getting total page no
	string page_url = argv[1];
	page_url += "/1";
    auto r = cpr::Get(cpr::Url{page_url});
	int total_pages = stoi(match(r.text, reg_total_pages));
	cout << "Total pages: " << total_pages << endl;
	ProgressBar progressBar(total_pages, 70);
	
	//main loop
	for (int i = 1; i <= total_pages;i++) {
		//constructing and getting each page url 
		string temp_page_url = argv[1];
		temp_page_url += ("/" + patch::to_string(i));
		auto temp_r = cpr::Get(cpr::Url{ temp_page_url });
		//searching the jpg partial url in response text and constructing the full url for download
	    string temp_jpg_url = "https://lecturenotes.in"+match(temp_r.text, reg_page_url);
		//downloading the jpg img
		dl_jpg(temp_jpg_url, patch::to_string(i));
		++progressBar;
		cout << "Page: " << i << "  ";
		progressBar.display();
}
	progressBar.done();

	//dl_jpg("https://lecturenotes.in/uploads/upload/note/8d/8djmG7OoAg/22-5b1fbff199859ee950086584be28405c.jpeg", patch::to_string(22));
    //cout << "Hello World!\n";
}
