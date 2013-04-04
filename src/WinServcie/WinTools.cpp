#ifdef WIN32
#include "WinTools.h"
#include <Windows.h>
wchar_t* char2wchar(const char* src){
    size_t srcsize = strlen(src) + 1;
    size_t newsize = srcsize;	
    size_t convertedChars = 0;
    wchar_t *wcstring;
	wcstring=new wchar_t[newsize];
    mbstowcs_s(&convertedChars, wcstring, srcsize, src, _TRUNCATE);
	return wcstring;
}
string wchar2char(const wchar_t *wchar){  
	char chr[1000];
	memset(chr,0,1000);
    WideCharToMultiByte( CP_ACP, 0, wchar, wcslen(wchar),  
        chr, 1000, NULL, NULL );
	string tmp;
	tmp.append(chr);
	return tmp;
}
string ModulePath(){
	wchar_t exeFullPath[MAX_PATH]={0}; // MAX_PATH在WINDEF.h中定义了，等于260
	GetModuleFileName(NULL,exeFullPath,MAX_PATH);
	string ss=wchar2char(exeFullPath);
	int pos=ss.find_last_of('\\');
	return ss.substr(0,pos+1);
}
string ModuleFullPath(){
	wchar_t exeFullPath[MAX_PATH]={0}; // MAX_PATH在WINDEF.h中定义了，等于260
	GetModuleFileName(NULL,exeFullPath,MAX_PATH);
	string ss=wchar2char(exeFullPath);
	return ss;
}
#endif