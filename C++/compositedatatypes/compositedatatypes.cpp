/*
    Sample using the composite data types (license required)
*/
// Include files to use the pylon API.
#include <pylon/PylonIncludes.h>
#include <stdlib.h>
#ifdef PYLON_WIN_BUILD
#  include <pylon/PylonGUI.h>
#endif
// Extend the pylon API for using pylon data processing.
#include <pylondataprocessing/PylonDataProcessingIncludes.h>
// The sample uses the std::list.
#include <list>
#include <cstdlib>
// Namespaces for using pylon objects
using namespace Pylon;
using namespace Pylon::DataProcessing;

// Namespace for using cout
using namespace std;

// Number of images to be grabbed
static const uint32_t c_countOfImagesToGrab = 24;

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
        TCHAR tRoot[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, tRoot);
        wstring wRoot(&tRoot[0]);
        string recipePath(wRoot.begin(), wRoot.end());
        recipePath += "\\composite_data_types.precipe";

        if (_dupenv_s(&buf, &sz, "PYLON_DEV_DIR") == 1 || buf == nullptr)
        {
            char pylon[50] = "C:\\Program Files\\Basler\\pylon 8\\Development";
            buf = pylon;
        }

        string pylonDir(buf);
        string imageDir = pylonDir + "\\Samples\\pylonDataProcessing\\C++\\images\\shapes";
        // This object is used for collecting the output data.
        // If placed on the stack, it must be created before the recipe
        // so that it is destroyed after the recipe.
        CGenericOutputObserver resultCollector;

        // Create a recipe object representing a recipe file created using
        // the pylon Viewer Workbench.
        CRecipe recipe;

        // Load the recipe file.        
        // created by the CMake build files.
        recipe.Load(recipePath.c_str());

        // Now we allocate all resources we need. This includes the camera device.
        recipe.PreAllocateResources();

        // Set up correct image path to samples.
        // Note: PYLON_DATAPROCESSING_SHAPE_IMAGES_PATH is a string created by the CMake build files.
        recipe.GetParameters().Get(StringParameterName("MyCamera/@CameraDevice/ImageFilename")).SetValue(imageDir.c_str());

        // This is where the output goes.
        recipe.RegisterAllOutputsObserver(&resultCollector, RegistrationMode_Append);

        // Start the processing.
        recipe.Start();

        for (uint32_t i = 0; i < c_countOfImagesToGrab; ++i)
        {
            if (resultCollector.GetWaitObject().Wait(5000))
            {
                CVariantContainer result = resultCollector.RetrieveResult();

                CVariant imageVariant = result["Image"];
                if (!imageVariant.HasError())
                {
#ifdef PYLON_WIN_BUILD
                    DisplayImage(1, imageVariant.ToImage());
#endif
                }
                else
                {
                    cout << "An error occurred during processing (pin 'Image'): " << imageVariant.GetErrorDescription() << endl;
                }

                CVariant boxesArray = result["Boxes"];
                if (!boxesArray.HasError())
                {
                    // Get the array's data values as SRectangleF data type.
                    vector<SRectangleF> boxes;
                    PYLON_ASSERT(boxesArray.GetDataType() == Pylon::DataProcessing::VariantDataType_RectangleF);
                    boxes.resize(boxesArray.GetNumArrayValues());
                    boxesArray.GetArrayDataValues(boxes.data(), boxes.size() * sizeof(SRectangleF));
                    cout << "########## Image " << i << " ##########" << endl << endl;
                    for (const SRectangleF& box : boxes)
                    {
                        cout << "RectangleF {" << endl
                            << "  Center: " << "{" << endl
                            << "    X: " << box.Center.X << "," << endl
                            << "    Y: " << box.Center.Y << endl
                            << "  }," << endl
                            << "  Width:    " << box.Width << "," << endl
                            << "  Height:   " << box.Height << "," << endl
                            << "  Rotation: " << box.Rotation << endl
                            << "}" << endl;
                    }

                    cout << endl << endl << endl;
                }
                else
                {
                    cout << "An error occurred during processing (pin 'Boxes'): " << boxesArray.GetErrorDescription() << endl;
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