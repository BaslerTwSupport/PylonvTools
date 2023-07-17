using Basler.Pylon;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using vTools.DotNet;
namespace barcode
{
    internal class Program
    {

        static void Main(string[] args)
        {
            vToolsImpl.PylonInitialize();
            vToolsImpl tools = new vToolsImpl();
            try
            {
                var pylonDir = Environment.GetEnvironmentVariable("PYLON_DEV_DIR");
                var recipeFile = $@"{Directory.GetParent(Directory.GetCurrentDirectory()).Parent.FullName}\barcode.precipe";
                
                tools.EnableCameraEmulator();
                tools.LoadRecipe(recipeFile);
                tools.SetParameters("MyCamera/@CameraDevice/ImageFilename", $@"{pylonDir}\Samples\pylonDataProcessing\C++\images\barcode\");
                tools.RegisterAllOutputsObserver();
                tools.Start();
                for (int i = 0; i < 10000; i++) 
                {
                    if(tools.WaitObject(5000) && tools.NextOutput())
                    {
                        var barcode = tools.GetStringArray("Barcodes");
                        var img = tools.GetImage("Image");
                        
                        Console.WriteLine($"Barcode: {string.Join(",", barcode)}");
                        ImageWindow.DisplayImage(0, img.Bytes, PixelType.Mono8, img.Width, img.Height, 0, ImageOrientation.TopDown);
                        Console.WriteLine(i);
                    }
                }
                //int result = tool.Sub();
            }
            catch (Win32Exception ex)
            {
                Console.WriteLine(ex);
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex);
            }
            finally
            {
                vToolsImpl.PylonTerminate();
            }
        }
    }
}
