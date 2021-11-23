#include "ApiRequestManager.h"

ApiRequestManager::ApiRequestManager(const Logger& _logger, string _apiKey, string _secretKey) : logger(_logger)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	apiKey = _apiKey;
	secretKey = _secretKey;
}

void ApiRequestManager::CleanUpApiRequestManager()
{
	logger->writeInfoEntry("Cleaning up the ApiRequestManager.");

	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

//void ApiRequestManager::GetAccountInformation()
//{
//	string post_data = "";
//	string str_result;
//	string action = "GET";
//
//	string url(BINANCE_HOST);
//	string queryString;
//}

/*
* make a webrequest
*/
void ApiRequestManager::CurlAPIWithHeader(string& _url, string& _strResult, vector <string>& _extraHttpHeader, string& _postData, string& _action, string _queryString)
{
	if (apiKey.size() == 0 || secretKey.size() == 0)
	{
		logger->writeWarnEntry("API Key or/and Secret Key has not been set.");
		return;
	}

	logger->writeInfoEntry("making cUrl call to " + _url);
	
	CURLcode res;

	if (curl)
	{
		vector <string> extra_http_header;
		SetSecretAndApiKeyInUrl(_url, extra_http_header, _queryString);

		curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ApiRequestManager::WebRequestCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_strResult);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

		if (_extraHttpHeader.size() > 0) 
		{
			struct curl_slist* chunk = NULL;
			for (int i = 0; i < _extraHttpHeader.size(); i++)
			{
				chunk = curl_slist_append(chunk, _extraHttpHeader[i].c_str());
			}
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		}

		if (_postData.size() > 0 || _action == "POST" || _action == "PUT" || _action == "DELETE") 
		{
			if (_action == "PUT" || _action == "DELETE") 
			{
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, _action.c_str());
			}

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _postData.c_str());
		}

		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK) 
		{
			logger->writeErrorEntry(curl_easy_strerror(res));
			cerr << "CURL error: " << res << endl;
		}
	}
	else
	{
		logger->writeErrorEntry("Could not initialize cURL.");
	}
}

void ApiRequestManager::SetSecretAndApiKeyInUrl(string& _url, vector <string>& _extraHttpHeader, string _querystring)
{
	string signature = HmacSha256(secretKey, _querystring);
	_querystring.append("&signature=");
	_querystring.append(signature);

	_url.append(_querystring);

	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append(apiKey);
	_extraHttpHeader.push_back(header_chunk);
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

string ApiRequestManager::HmacSha256(const string _secretKey, const string _querystring)
{
	unsigned char* digest;
	digest = HMAC(EVP_sha256(), secretKey.c_str(), strlen(secretKey.c_str()), (unsigned char*)_querystring.c_str(), strlen(_querystring.c_str()), NULL, NULL);
	return B2AHex((char*)digest, 32);
}

string ApiRequestManager::B2AHex(char* _byteArray, int _n)
{
	const static string hexCodes = "0123456789abcdef";
	string hexString;
	for (int i = 0; i < 32; ++i) {
		unsigned char BinValue = _byteArray[i];
		hexString += hexCodes[(BinValue >> 4) & 0x0F];
		hexString += hexCodes[BinValue & 0x0F];
	}

	return hexString;
}