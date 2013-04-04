#ifdef WIN32
#include "WinService.h"
#include "WinTools.h"
#include <sstream>
#include <Windows.h>
string InstallService(string name,string epath,vector<string> args){
	printf("Begin Install...\n");
	stringstream ss;
	ss<<epath;
	for(size_t i=0;i<args.size();i++){
		ss<<" "<<args[i];
	}
	wchar_t * szPath=char2wchar(ss.str().c_str());
	SC_HANDLE schService; 
	SC_HANDLE schSCManager; 
	schSCManager = OpenSCManager(0,0,SC_MANAGER_CREATE_SERVICE);
	if (!schSCManager){
		return string("OpenScManagerErrorID:"+GetLastError());  
	}
	// Install new services.
	wchar_t* sname=char2wchar(name.c_str());
	schService = CreateService(
		schSCManager, // handle to service control manager database
		sname, // pointer to name of service to start
		sname, // pointer to display name     
		SERVICE_ALL_ACCESS,// type of access to service
		SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS ,// type of service
		SERVICE_DEMAND_START,// when to start service
		SERVICE_ERROR_NORMAL,// severity if service fails to start
		szPath,       // pointer to name of binary file
		NULL,         // pointer to name of load ordering group
		NULL,         // pointer to variable to get tag identifier
		NULL,         // pointer to array of dependency names
		NULL,         // pointer to account name of service
		NULL          // pointer to password for service account
		);
	delete sname,sname=NULL;
	if (!schService){  
		return string("CreateServiceErrorID:"+GetLastError()); 
	} 
	// clean up
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	printf("Install Ending...\n");
	return "";
}
string UnInstallService(string name){
	printf("Begin Uninstall Service:%s\n",name.c_str());
	SC_HANDLE schService;
	SC_HANDLE schSCManager; 
	BOOL success;
	SERVICE_STATUS svcStatus;
	// open to SCM　
	schSCManager = OpenSCManager(
		0,// pointer to machine name string 
		0,// pointer to database name string
		SC_MANAGER_CREATE_SERVICE // type of access
		);
	if (!schSCManager){
		return string("OpenScManagerErrorID:"+GetLastError());
	}
	wchar_t* sname=char2wchar(name.c_str());
	//打开一个服务
	schService = OpenService(
		schSCManager,              // handle to service control manager database
		sname,              // pointer to name of service to start
		SERVICE_ALL_ACCESS | DELETE// type of access to service
		);
	delete sname,sname=NULL;
	if (!schService)
	{
		return string("OpenServiceErrorID:"+GetLastError());
	}
	//(if necessary)
	success = QueryServiceStatus(schService, &svcStatus);
	if (!success)
	{
		return string("In QueryServiceStatus ErrorID:"+GetLastError());
	}
	if (svcStatus.dwCurrentState != SERVICE_STOPPED){
		printf("stopping service...\n");
		success = ControlService(
			schService,           // handle to service
			SERVICE_CONTROL_STOP, // control code
			&svcStatus            // pointer to service status structure
			);
		if (!success){
			return string("In ControlServiceErrorID:"+GetLastError());
		}
	}
	//等待
	do{
		QueryServiceStatus(schService,&svcStatus);      
		Sleep(500);    

	}while(SERVICE_STOP_PENDING==svcStatus.dwCurrentState);        
	if(SERVICE_STOPPED!=svcStatus.dwCurrentState){
		return string("Failed to Stop Service ErrorID:"+GetLastError());  
	}
	//删除服务 
	success = DeleteService(schService);
	if (success){
		printf("service %s removed\n",name.c_str());
	}
	else{
		return string("In DeleteService ErrorID:"+GetLastError());
	}	 
	//Clean up
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);	 
	printf("Uninstal Ending...\n");
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE SsHandle;
HANDLE TerminateEvent; //事件
HANDLE ThreadHandle;   //线程
wchar_t* _sname=NULL;
int _argc=0;
char** _argv=0;
ServicesMethod _smm=NULL;
ServicesMethod _sim=NULL;
ServicesMethod _stm=NULL;
void ControlHandler(DWORD request){
   switch(request){ 
	   case SERVICE_CONTROL_STOP:
			ServiceStatus.dwCurrentState=SERVICE_STOP_PENDING;
			SetServiceStatus(SsHandle,&ServiceStatus);
			SetEvent(TerminateEvent);
			if(_stm){
				 _stm(_argc,_argv);
			 }
			break;
	   case SERVICE_CONTROL_INTERROGATE:
			break;
	   case SERVICE_CONTROL_SHUTDOWN:
			 SetEvent(TerminateEvent);
			 if(_stm){
				 _stm(_argc,_argv);
			 }
			return;
	   default:
			break;
   }   
}
BOOL InitService(wchar_t *sname){
	SC_HANDLE schSCManager;
	SC_HANDLE scHandle;
	BOOL boolRet;
	// open to SCM   
	schSCManager = OpenSCManager(0,0,SC_MANAGER_ALL_ACCESS);
	//Open
	scHandle = OpenService(schSCManager,sname,SERVICE_ALL_ACCESS);
	//start 
	boolRet = StartService(scHandle,0,NULL);  
	SsHandle = RegisterServiceCtrlHandler(sname,(LPHANDLER_FUNCTION)ControlHandler);   
	if(!SsHandle)
	{	
		printf("ServiceMain函数中注册服务控制处理函数失败!\n");
		return 0;
	}
	ServiceStatus.dwCurrentState=SERVICE_RUNNING;
	ServiceStatus.dwServiceType=SERVICE_WIN32;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
		SERVICE_ACCEPT_PAUSE_CONTINUE |
		SERVICE_ACCEPT_SHUTDOWN;
	TerminateEvent = CreateEvent (0,TRUE,FALSE,0);
	if(!TerminateEvent){
		printf("create event fail!");
		return 0;
	}
	return 1;
}
BOOL SendStatusToScm (DWORD dwCurrentState,
					  DWORD dwWin32ExitCode, 
					  DWORD dwServiceSpecificExitCode,
					  DWORD dwCheckPoint,
					  DWORD dwWaitHint)
{
	BOOL success;
	SERVICE_STATUS serviceStatus;
	// Fill in all of the SERVICE_STATUS fields
	serviceStatus.dwServiceType =SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = dwCurrentState;

	// If in the process of something, then accept
	// no control events, else accept anything
	if (dwCurrentState == SERVICE_START_PENDING)
		serviceStatus.dwControlsAccepted = 0;
	else
		serviceStatus.dwControlsAccepted =SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;

	// if a specific exit code is defines, set up
	// the win32 exit code properly
	if (dwServiceSpecificExitCode == 0)
		serviceStatus.dwWin32ExitCode =dwWin32ExitCode;
	else
		serviceStatus.dwWin32ExitCode =	ERROR_SERVICE_SPECIFIC_ERROR;
	serviceStatus.dwServiceSpecificExitCode =dwServiceSpecificExitCode;

	serviceStatus.dwCheckPoint = dwCheckPoint;
	serviceStatus.dwWaitHint = dwWaitHint;

	// Pass the status record to the SCM
	//SERVICE_STATUS_HANDLE serviceStatusHandle;
	success = SetServiceStatus (SsHandle,&serviceStatus);
	return success;
}
void Terminate(DWORD error)
{
	// if terminateEvent has been created, close it.
	if (TerminateEvent)
		CloseHandle(TerminateEvent);

	// Send a message to the scm to tell about
	// stop age
	if (SsHandle)
		SendStatusToScm(SERVICE_STOPPED, error, 0, 0, 0);

	// If the thread has started kill it off
	if (ThreadHandle)
		CloseHandle(ThreadHandle);

	if(_sname){
		delete _sname;
		_sname=NULL;
	}
	// Do not need to close serviceStatusHandle
}
void ServiceMain(int argc,char** argv){
	int success=true;
	success=InitService(_sname);
	if(!success){
		Terminate(GetLastError()); 
	}
	success =  SendStatusToScm(SERVICE_START_PENDING,NO_ERROR,0,2,1000);
	if (!success)
	{
		Terminate(GetLastError()); 
		return;
	}
	if(_sim){
		_sim(_argc,_argv);
	}
	success = SendStatusToScm(SERVICE_RUNNING,NO_ERROR,0,0,0);
	if (!success){
		Terminate(GetLastError()); 
		return;
	}
	if(_smm){
		_smm(_argc,_argv);
	}
	ServiceStatus.dwCurrentState=SERVICE_STOPPED;
	SetServiceStatus(SsHandle,&ServiceStatus);
	Terminate(0);
}
void SetServicesInitMethod(ServicesMethod sim){
	_sim=sim;
}
void SetServicesMainMethod(ServicesMethod smm){
	_smm=smm;
}
void SetServicesStopMethod(ServicesMethod stm){
	_stm=stm;
}
int RunService(string name,int argc,char** argv){
	_sname=char2wchar(name.c_str());
	_argc=argc;
	_argv=argv;
	SERVICE_TABLE_ENTRY serviceTable[2]; 
	serviceTable[0].lpServiceName=_sname;
	serviceTable[0].lpServiceProc=(LPSERVICE_MAIN_FUNCTION)ServiceMain;
	serviceTable[1].lpServiceName=NULL;
	serviceTable[1].lpServiceProc=NULL;
	StartServiceCtrlDispatcher(serviceTable);
	return 0;
}
/*
fstream* sdl=0;
void initSdl(string p){
	sdl=new fstream(p.c_str(),ios::app);
}
void freeSdl(){
	if(sdl){
		sdl->close();
		delete sdl;
	}
}
*/
#endif