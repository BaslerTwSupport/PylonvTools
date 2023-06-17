#ifdef VTOOLSWRAPPER_EXPORTS //�P�M�צW�١A�u�O�᭱�T�w��_EXPORTS
#define VTOOLSWRAPPER_API __declspec(dllexport) //�Ъ`�N�I���T���OExport�n�G�_
#else
#define VTOOLSWRAPPER_API __declspec(dllimport)
#endif
#include "vTools.h"

extern "C"
{
	static vTools* tools = NULL;

	VTOOLSWRAPPER_API bool EnableCameraEmulator()
	{
		_putenv("PYLON_CAMEMU=1");
		return 1;
	}

	VTOOLSWRAPPER_API bool LoadRecipe(char* fileName)
	{
		tools = new vTools();
		tools->LoadRecipe(string(fileName));
		return 1;
	}
	VTOOLSWRAPPER_API bool SetParameters(char* name, char* value)
	{
		tools->SetParameters(name, value);
		return 1;
	}
	VTOOLSWRAPPER_API bool RegisterAllOutputsObserver()
	{
		tools->RegisterAllOutputsObserver();
		return 1;
	}
	VTOOLSWRAPPER_API bool Start()
	{
		tools->Start();
		return 1;
	}
	VTOOLSWRAPPER_API bool WaitObject(unsigned int timeOut)
	{
		return tools->WaitObject(timeOut);
	}
	VTOOLSWRAPPER_API bool Stop()
	{
		tools->Stop();
		return 1;
	}
	VTOOLSWRAPPER_API bool SetRecipeInput(char* name, char* value)
	{
		tools->SetRecipeInput(name, value);
		return 1;
	}
	VTOOLSWRAPPER_API bool Dispose()
	{
		tools->Dispose();
		return 1;
	}

	VTOOLSWRAPPER_API bool NextOutput()
	{
		return tools->ResultCollector.NextOutput();
	}

	VTOOLSWRAPPER_API byte* GetImage(char* name, int* w, int* h, int* channels)
	{
		auto cimg = tools->ResultCollector.GetImage(name, w, h, channels);
		int imgW = *w;
		int imgH = *h;
		int imgC = *channels;
		int size = imgW * imgH * imgC;
		byte* result = (byte*)malloc(size * sizeof(byte*));
		memcpy(result, cimg.GetBuffer(), size);
		cimg.Release();
		return result;
	}
	VTOOLSWRAPPER_API const char* GetString(char* name)
	{
		return tools->ResultCollector.GetString(name);
	}

	VTOOLSWRAPPER_API const char** GetStringArray(char* name, int* num)
	{	
		return tools->ResultCollector.GetStringList(name, num);
	}

	VTOOLSWRAPPER_API void Free(void* ptr)
	{
		free(ptr);
	}
}