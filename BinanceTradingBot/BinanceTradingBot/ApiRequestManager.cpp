#include "ApiRequestManager.h"

/*
* make a webrequest
*/
void ApiRequestManager::CurlAPIWithHeader(string& url, string& str_result, vector <string>& extra_http_header, string& post_data, string& action)
{
	logger->writeInfoEntry("making cUrl call to " + url);

	string result;
	CURL* curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ApiRequestManager::my_write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

		if (extra_http_header.size() > 0) 
		{

			struct curl_slist* chunk = NULL;
			for (int i = 0; i < extra_http_header.size(); i++) 
			{
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str());
			}
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		}

		if (post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE") 
		{
			if (action == "PUT" || action == "DELETE") 
			{
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, action.c_str());
			}

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
		}

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);

		/* Check for errors */
		if (res != CURLE_OK) 
		{
			std::ostringstream oss;
			oss << "curl_easy_perform() failed: " << curl_easy_strerror(res);

			logger->writeErrorEntry(oss.str());
			cerr << "CURL error: " << res << endl;
		}
	}

	logger->writeInfoEntry(result);
}

/*
* Callbackfunction to retrieve the webrequest
*/
size_t ApiRequestManager::my_write(void* content, size_t size, size_t nmemb, string* buffer)
{
	size_t totalsize = size * nmemb;
	buffer->append((char*)content, totalsize);

	return totalsize;
}