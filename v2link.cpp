#include <windows.h>
#include "tp_stub.h"

#define EXPORT(hr) extern "C" __declspec(dllexport) hr __stdcall

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:V2Link=_V2Link@4")
#pragma comment(linker, "/EXPORT:V2Unlink=_V2Unlink@0")
#endif



#ifdef _WIN32

//--------------------------------------
HINSTANCE DllHandle = NULL;
BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID lpReserved)
{
	switch (nReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hDllHandle);
			DllHandle = hDllHandle;
			break;
		}
	}

	return TRUE;
}

#if 0
static iTVPFunctionExporter * FunctionExporter = NULL;

//---------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#ifndef __clang__
#include <cxxabi.h>
#endif

static void std_terminate_handler()
{
	std::stringstream ss;
	auto const ep = std::current_exception();
	if (ep)
	{
		ss << "Unhandled exception from plugin";
		if (DllHandle != NULL)
		{
			ss << " ";
			{
				CHAR* modnamebuf = (CHAR*)malloc(32768 * sizeof(CHAR));
				if (modnamebuf)
				{
					DWORD retLen = GetModuleFileNameA(DllHandle, modnamebuf, 32768);
					if (retLen)
					{
						ss << modnamebuf;
					}
					free(modnamebuf);
				}
			}
		}
		ss << ":";
		try
		{
			ss << "Terminating with uncaught exception";
#ifndef __clang__
			int status;
			auto const etype = abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(), 0, 0, &status);
			if (etype != NULL)
			{
				ss << " of type `" << etype << "`";
			}
#endif
			std::rethrow_exception(ep);
		}
		catch(const std::exception& e)
		{
			ss << " with `what()` = \"" << e.what() << "\"";
		}
		catch(...) {}
		std::cerr << ss.str();
		if (FunctionExporter != NULL)
		{
			ttstr ssttstr(ss.str().c_str());
			TVPThrowExceptionMessage(ssttstr.c_str());
		}
	}
	std::abort();
}
#endif
#endif


static tjs_int GlobalRefCountAtInit = 0;

extern bool onV2Link();
extern bool onV2Unlink();

EXPORT(HRESULT) V2Link(iTVPFunctionExporter *exporter)
{
	TVPInitImportStub(exporter);
#if 0
#ifdef _WIN32
	FunctionExporter = exporter;
	std::set_terminate(std_terminate_handler);
#endif
#endif

	if (!onV2Link()) return E_FAIL;

	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	return S_OK;
}
EXPORT(HRESULT) V2Unlink()
{
	if (TVPPluginGlobalRefCount > GlobalRefCountAtInit ||
		!onV2Unlink()) return E_FAIL;

	TVPUninitImportStub();
	return S_OK;
}

