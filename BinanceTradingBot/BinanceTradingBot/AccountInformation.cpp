#include "AccountInformation.h"

/*
* 
*/
void AccountInformation::GetAccountInformation(string _secretKey, string _apiKey, unsigned short _recvWindow)
{
	//ostringstream oss;
	//oss << __func__ << " is getting called...";
	//
	//logger->writeInfoEntry(oss.str());
	//oss.clear();
	//
	//auto test = __func__;
	//
	//if (_apiKey.size() == 0 || _secretKey.size() == 0) {
	//
	//	oss << __func__ << " is getting called...";
	//
	//
	//	logger->writeWarnEntry("GetAccountInformation() is getting called..." );
	//	//BinaCPP_logger::write_log("<BinaCPP::get_account> API Key and Secret Key has not been set.");
	//	return;
	//}
	//
	//string url(BINANCE_HOST);
	//url += "/api/v3/account?";
	//string action = "GET";
	//
	//chrono::milliseconds ms = duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
	//
	//// When the request got created
	//string querystring("timestamp=");
	//querystring.append(to_string(ms.count()));
	//
	//// For how long the request is valid
	//if (_recvWindow > 0) {
	//	querystring.append("&recvWindow=");
	//	querystring.append(to_string(_recvWindow));
	//}
	//
	//unsigned char* digest;
	//digest = HMAC(EVP_sha256(), _secretKey.c_str(), strlen(_secretKey.c_str()), (unsigned char*)querystring.c_str(), strlen(querystring.c_str()), NULL, NULL);
	//
	//const static string HexCodes = "0123456789abcdef";
	//string HexString;
	//for (int i = 0; i < 32; ++i) {
	//	unsigned char BinValue = digest[i];
	//	HexString += HexCodes[(BinValue >> 4) & 0x0F];
	//	HexString += HexCodes[BinValue & 0x0F];
	//}
	//
	//string signature = HexString;
	//querystring.append("&signature=");
	//querystring.append(signature);
	//
	//url.append(querystring);
	//
	//vector <string> extra_http_header;
	//string header_chunk("X-MBX-APIKEY: ");
	//header_chunk.append(_apiKey);
	//extra_http_header.push_back(header_chunk);
	//
	//string post_data = "";


	//curl_global_cleanup();
	//
	//logger->writeInfoEntry(result);
}
