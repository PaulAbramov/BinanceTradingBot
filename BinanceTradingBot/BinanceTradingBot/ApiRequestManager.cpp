#include "ApiRequestManager.h"

/*
* make a webrequest
*/
void ApiRequestManager::CurlAPIWithHeader(string& _url, string& _str_result, vector <string>& _extra_http_header, string& _post_data, string& _action)
{
	logger->writeInfoEntry("making cUrl call to " + _url);

	string result;
	CURL* curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ApiRequestManager::WebRequestCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

		if (_extra_http_header.size() > 0) 
		{
			struct curl_slist* chunk = NULL;
			for (int i = 0; i < _extra_http_header.size(); i++) 
			{
				chunk = curl_slist_append(chunk, _extra_http_header[i].c_str());
			}
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		}

		if (_post_data.size() > 0 || _action == "POST" || _action == "PUT" || _action == "DELETE") 
		{
			if (_action == "PUT" || _action == "DELETE") 
			{
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, _action.c_str());
			}

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _post_data.c_str());
		}

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);

		/* Check for errors */
		if (res != CURLE_OK) 
		{
			ostringstream oss;
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
size_t ApiRequestManager::WebRequestCallback(void* _content, size_t _size, size_t _nmemb, string* _buffer)
{
	size_t totalsize = _size * _nmemb;
	_buffer->append((char*)_content, totalsize);

	return totalsize;
}