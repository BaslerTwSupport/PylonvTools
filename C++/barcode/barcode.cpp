/*
	Sample using the Barcode vTool (license required)
*/
// Include files to use the pylon API.
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#  include <pylon/PylonGUI.h>
#endif
// Extend the pylon API for using pylon data processing.
#include <pylondataprocessing/PylonDataProcessingIncludes.h>
// The sample uses the std::list.
#include <list>
#include <iostream>
#include <filesystem>
// Namespaces for using pylon objects
using namespace Pylon;
using namespace Pylon::DataProcessing;

// Namespace for using cout
using namespace std;
namespace fs = std::filesystem;
// Number of images to be grabbed
static const uint32_t c_countOfImagesToGrab = 100;

// Declare a data class for one set of output data values.
class ResultData
{
public:
	ResultData()
		: hasError(false)
	{
	}

	CPylonImage image; // The image from the camera
	CPylonImage region;
	StringList_t decodedBarcodes; // The decoded barcodes

	bool hasError;     // If something doesn't work as expected
					   // while processing data, this is set to true.

	String_t errorMessage; // Contains an error message if
						   // hasError has been set to true.
};

// MyOutputObserver is a helper object that shows how to handle output data
// provided via the IOutputObserver::OutputDataPush interface method.
// Also, MyOutputObserver shows how a thread-safe queue can be implemented
// for later processing while pulling the output data.
class MyOutputObserver : public IOutputObserver
{
public:
	MyOutputObserver()
		: m_waitObject(WaitObjectEx::Create())
	{
	}

	// Implements IOutputObserver::OutputDataPush.
	// This method is called when an output of the CRecipe pushes data out.
	// The call of the method can be performed by any thread of the thread pool of the recipe.
	void OutputDataPush(
		CRecipe& recipe,
		CVariantContainer valueContainer,
		const CUpdate& update,
		intptr_t userProvidedId) override
	{
		// The following variables are not used here:
		PYLON_UNUSED(recipe);
		PYLON_UNUSED(update);
		PYLON_UNUSED(userProvidedId);

		ResultData currentResultData;

		// Get the result data of the recipe via the output terminal's "Image" pin.
		// The value container is a dictionary/map-like type.
		// Look for the key in the dictionary.
		auto posImage = valueContainer.find("Image");
		auto num = valueContainer.size();
		cout << "Container count:" << valueContainer.size() << endl;
		
		// We expect there to be an image
		// because the recipe is set up to behave like this.
		PYLON_ASSERT(posImage != valueContainer.end());
		if (posImage != valueContainer.end())
		{
			// Now we can use the value of the key/value pair.
			const CVariant& value = posImage->second;
			if (!value.HasError())
			{
				currentResultData.image = value.ToImage();
			}
			else
			{
				currentResultData.hasError = true;
				currentResultData.errorMessage = value.GetErrorDescription();
			}
		}

		// Get the data from the Barcodes pin.
		auto posBarcodes = valueContainer.find("Barcodes");
		PYLON_ASSERT(posBarcodes != valueContainer.end());
		if (posBarcodes != valueContainer.end())
		{
			const CVariant& value = posBarcodes->second;
			if (!value.HasError())
			{
				for (size_t i = 0; i < value.GetNumArrayValues(); ++i)
				{
					const CVariant decodedBarcodeValue = value.GetArrayValue(i);
					if (!decodedBarcodeValue.HasError())
					{
						currentResultData.decodedBarcodes.push_back(decodedBarcodeValue.ToString());
					}
					else
					{
						currentResultData.hasError = true;
						currentResultData.errorMessage = value.GetErrorDescription();
						break;
					}
				}
			}
			else
			{
				currentResultData.hasError = true;
				currentResultData.errorMessage = value.GetErrorDescription();
			}
		}


		auto regions = valueContainer.find("RecipeOutput");
		PYLON_ASSERT(regions != valueContainer.end());
		if (posBarcodes != valueContainer.end())
		{
			const CVariant& value = regions->second;
			if (!value.HasError())
			{
				for (size_t i = 0; i < 1; ++i)
				{
					const CVariant region = value.GetArrayValue(i);
					if (!region.HasError())
					{
						auto types = region.GetDataType();
						cout << "Type: " << region.GetDataType() << endl;
					}
					else
					{
						currentResultData.hasError = true;
						currentResultData.errorMessage = value.GetErrorDescription();
						break;
					}
				}
			}
		}
		// Add data to the result queue in a thread-safe way.
		{
			AutoLock scopedLock(m_memberLock);
			m_queue.emplace_back(currentResultData);
		}

		// Signal that data is ready.
		m_waitObject.Signal();
	}

	// Get the wait object for waiting for data.
	const WaitObject& GetWaitObject()
	{
		return m_waitObject;
	}

	// Get one result data object from the queue.
	bool GetResultData(ResultData& resultDataOut)
	{
		AutoLock scopedLock(m_memberLock);
		if (m_queue.empty())
		{
			return false;
		}

		resultDataOut = std::move(m_queue.front());
		m_queue.pop_front();
		if (m_queue.empty())
		{
			m_waitObject.Reset();
		}
		return true;
	}

private:
	CLock m_memberLock;        // The member lock is required to ensure
							   // thread-safe access to the member variables.
	WaitObjectEx m_waitObject; // Signals that ResultData is available.
							   // It is set if m_queue is not empty.
	list<ResultData> m_queue;  // The queue of ResultData
};

int main(int /*argc*/, char* /*argv*/[])
{
	// The exit code of the sample application.
	int exitCode = 0;

	// Enable the pylon camera emulator to provide images from disk
	// by setting the necessary environment variable.
#if defined(PYLON_WIN_BUILD)
	_putenv("PYLON_CAMEMU=1");
#elif defined(PYLON_UNIX_BUILD)
	setenv("PYLON_CAMEMU", "1", true);
#endif

	// Before using any pylon methods, the pylon runtime must be initialized.
	PylonInitialize();
	char* buf = nullptr;
	size_t sz = 0;
	try
	{
		// This object is used for collecting the output data.
		// If placed on the stack, it must be created before the recipe
		// so that it is destroyed after the recipe.
		MyOutputObserver resultCollector;

		// Create a recipe object representing a recipe file created using
		// the pylon Viewer Workbench.
		CRecipe recipe;

		// Load the recipe file.
		// Note: PYLON_DATAPROCESSING_BARCODE_RECIPE is a string
		// created by the CMake build files.
		auto recipePath = fs::current_path().string() + "\\barcode.precipe";

		if (_dupenv_s(&buf, &sz, "PYLON_DEV_DIR") == 1 || buf == nullptr)
		{
			char pylon[50] = "C:\\Program Files\\Basler\\pylon 7\\Development";
			buf = pylon;
		}

		string pylonDir(buf);
		string imageDir = pylonDir + "\\Samples\\pylonDataProcessing\\C++\\images\\barcode\\";
		recipe.Load(recipePath.c_str());
		// Now we allocate all resources we need. This includes the camera device.
		recipe.PreAllocateResources();

		// Set up correct image path to samples.
		// Note: PYLON_DATAPROCESSING_IMAGES_PATH is a string created by the CMake build files.
		recipe.GetParameters().Get(StringParameterName("MyCamera/@CameraDevice/ImageFilename")).SetValue(imageDir.c_str());

		// This is where the output goes.
		recipe.RegisterAllOutputsObserver(&resultCollector, RegistrationMode_Append);
		
		// Start the processing.
		recipe.Start();

		//for (uint32_t i = 0; i < c_countOfImagesToGrab; ++i)
		while(true)
		{
			if (resultCollector.GetWaitObject().Wait(5000))
			{
				ResultData result;
				resultCollector.GetResultData(result);
				if (!result.hasError)
				{
					// Access the image data.
					cout << "SizeX: " << result.image.GetWidth() << endl;
					cout << "SizeY: " << result.image.GetHeight() << endl;
					const uint8_t* pImageBuffer = (uint8_t*)result.image.GetBuffer();
					cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;
					cout << "Barcodes:" << endl;
					int index = 1;
					for (const auto& barcode : result.decodedBarcodes)
					{
						cout << index << " : " << barcode << endl;
						index++;
					}
					cout << endl << endl << endl;

#ifdef PYLON_WIN_BUILD
					DisplayImage(1, result.image);
#endif
				}
				else
				{
					cout << "An error occurred during processing: " << result.errorMessage << endl;
				}
			}
			else
			{
				throw RUNTIME_EXCEPTION("Result timeout");
			}
		}

		// Stop the processing.
		recipe.Stop();
	}
	catch (const GenericException& e)
	{
		// Error handling
		cerr << "An exception occurred." << endl << e.GetDescription() << endl;
		exitCode = 1;
	}

	// Comment the following two lines to disable waiting on exit.
	cerr << endl << "Press enter to exit." << endl;
	while (cin.get() != '\n');

	// Releases all pylon resources.
	PylonTerminate();

	return exitCode;
}
