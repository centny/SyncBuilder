#ifdef WIN32
#pragma once
#include <string>
#include <vector>
#include <fstream>
using namespace std;
typedef int (*ServicesMethod)(int argc,char** argv);
string InstallService(string name,string epath,vector<string> args);
string UnInstallService(string name);
void SetServicesInitMethod(ServicesMethod sim);
void SetServicesMainMethod(ServicesMethod smm);
void SetServicesStopMethod(ServicesMethod stm);
int RunService(string name,int argc,char** argv);
/*
extern fstream* sdl;
#define Sdl (*sdl)
void initSdl(string path);
void freeSdl();
*/
#endif