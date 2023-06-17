#ifdef CONVERTERWRAPPER_EXPORTS //同專案名稱，只是後面固定為_EXPORTS
#define CONVERTERWRAPPER_API __declspec(dllexport) //請注意！正確的是Export要亮起
#else
#define CONVERTERWRAPPER_API __declspec(dllimport)
#endif
#include <pylon/PylonIncludes.h>
// Extend the pylon API for using pylon data processing.
#include <pylondataprocessing/PylonDataProcessingIncludes.h>
using namespace Pylon;
using namespace Pylon::DataProcessing;
using namespace std;
extern "C" CONVERTERWRAPPER_API const char* GetStringT(char* fileName, char* values)
{
	String_t value(fileName);
	//int len = strlen(value.c_str());
	//*values = new char[len + 1];
	//*values = (char*)malloc(value.length() * sizeof(char*));
	//*values = fileName;
	//memcpy(*values, value.c_str(), len+1);
	//delete[] value;
	//*values = values1;
	int len = MultiByteToWideChar(CP_ACP, 0, value.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, value.c_str(), -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

extern "C" CONVERTERWRAPPER_API uint8_t * GetImage(uint8_t * image, int w, int h, int* imgW, int* imgH, int* imgC)
{
	CPylonImage cimg;
	cimg.AttachUserBuffer(image, w * h, PixelType_Mono8, w, h, 0, ImageOrientation_TopDown);
	uint8_t* result = (uint8_t*)malloc(w * h * sizeof(uint8_t*));
	memcpy(result, cimg.GetBuffer(), w * h);
	*imgW = w;
	*imgH = h;
	*imgC = 3;
	return result;
}
