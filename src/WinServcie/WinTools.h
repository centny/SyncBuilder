#ifdef WIN32
#pragma once
#include <string>
using namespace std;
//
wchar_t* char2wchar(const char* src);
string wchar2char(const wchar_t *wchar);
string ModulePath();
string ModuleFullPath();

#endif