#pragma once

#include <string>

using namespace std;

struct Config
{
public:
	string taapi_secret;
	string api_key;
	string secret_key;
	string gnuplot_path;
};
