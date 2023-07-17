using System;
using System.IO;
using vTools.DotNet.Models;

namespace vTools.DotNet
{
    public class vToolsImpl : IDisposable
    {
        private vToolsDotNet _tools;
        /// <summary>
        /// Initializes the pylon runtime system.
        /// </summary>
        public static void PylonInitialize() => vToolsDotNet.PylonInitialize();
        /// <summary>
        /// Frees up resources allocated by the pylon runtime system.
        /// </summary>
        public static void PylonTerminate() => vToolsDotNet.PylonTerminate();
        public vToolsImpl() 
        {
            _tools = new vToolsDotNet();
        }
        ~vToolsImpl()
        {
            Dispose();
        }
        /// <summary>
        /// Enable camera emulator. Create a vitural caemra. But it needs to set image files folder.
        /// </summary>
        /// <returns></returns>
        public void EnableCameraEmulator()=> _tools.EnableCameraEmulator();
        /// <summary>
        /// Load vTools recipe file.
        /// </summary>
        /// <param name="fileName"></param>
        /// <returns></returns>
        /// <exception cref="NullReferenceException"></exception>
        public void LoadRecipe(string fileName) 
        {
            if (!File.Exists(fileName))
            {
                throw new NullReferenceException($"{fileName} not exist!");
            }
            _tools.LoadRecipe(fileName);
        }
        /// <summary>
        /// Directly Set paratmers to operator.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public void SetParameters(string name, string value)=> _tools.SetParameters(name, value);
 
        /// <summary>
        /// Register all outputs observer.
        /// </summary>
        /// <returns></returns>
        public void RegisterAllOutputsObserver() => _tools.RegisterAllOutputsObserver();

        /// <summary>
        /// Recipe start.
        /// </summary>
        /// <returns></returns>
        public void Start() => _tools.Start();

        /// <summary>
        /// Wait next output result.
        /// </summary>
        /// <param name="timeout"></param>
        /// <returns></returns>
        public bool WaitObject(uint timeout) => _tools.WaitObject(timeout);

        /// <summary>
        /// Recipe stop.
        /// </summary>
        /// <returns></returns>
        public void Stop() => _tools.Stop();

        /// <summary>
        /// Set input value by string.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void SetString(string name, string value) => _tools.SetString(name, value);

        /// <summary>
        /// Set input value by boolean.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void SetBool(string name, bool value) => _tools.SetBool(name, value);

        /// <summary>
        /// Set input value by intger.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void SetLong(string name, int value) => _tools.SetLong(name, value);

        /// <summary>
        /// Set input value by double.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public void SetDouble(string name, double value) => _tools.SetDouble(name, value);
        public void Dispose() =>_tools.Dispose();

        public bool NextOutput() => _tools.NextOutput();

        public Img GetImage(string name)
        {
            var bytes = _tools.GetImage(name, out int w, out int h, out int c);
            return new Img(bytes, w, h, c);
        }

        public string GetString(string name) => _tools.GetString(name);

        public bool GetBool(string name) => _tools.GetBool(name);


        public long GetLong(string name) => _tools.GetLong(name);


        public double GetDouble(string name)
        {
            return _tools.GetDouble(name);
        }

        public Point GetPoint(string name)
        {
            _tools.GetPointF(name, out double x, out double y);
            return new Point(x, y);
        }

        public Rectangle GetRectangle(string name)
        {
            _tools.GetRectangleF(name, out double cenX, out double cenY, out double width, out double height, out double angle);
            return new Rectangle(new Point(cenX, cenY), new Size(width, height), angle);
        }

        public Circle GetCircle(string name)
        {
            _tools.GetCircleF(name, out double cenX, out double cenY, out double radius);
            return new Circle(new Point(cenX, cenY), radius);
        }

        public Ellipse GetEllipse(string name)
        {
            _tools.GetEllipseF(name, out double cenX, out double cenY, out double radius1, out double radius2, out double angle);
            return new Ellipse(new Point(cenX, cenY), radius1, radius2, angle);
        }

        public Line GetLine(string name)
        {
            _tools.GetLineF(name, out double x1, out double y1, out double x2, out double y2);
            return new Line(new Point(x1, x2), new Point(y1, y2));
        }

        public string[] GetStringArray(string name)
        {
            return _tools.GetStringArray(name);
        }

        public bool[] GetBoolArray(string name)
        {
            return _tools.GetBoolArray(name);
        }

        public long[] GetLongArray(string name)
        {
            return _tools.GetLongArray(name);
        }

        public double[] GetDoubleArray(string name)
        {
            return _tools.GetDoubleArray(name);
        }

        public Point[] GetPointArray(string name)
        {
            _tools.GetPointFArray(name, out double[] valuesX, out double[] valuesY);
            var num = valuesX.Length;
            var values = new Point[num];

            for (int i = 0; i < num; i++)
            {
                values[i] = new Point(valuesX[i], valuesY[i]);
            }
            return values;
        }

        public Rectangle[] GetRectangleArray(string name)
        {
            _tools.GetRectangleFArray(name, out double[] valuesX, out double[] valuesY, out double[] valuesW, out double[] valuesH, out double[] valuesA);
            var num = valuesX.Length;
            var values = new Rectangle[num];
            for (int i = 0; i < num; i++)
            {
                var p1 = new Point(valuesX[i], valuesY[i]);
                var size = new Size(valuesW[i], valuesH[i]);
                values[i] = new Rectangle(p1, size, valuesA[i]);
            }
            return values;
        }

        public Circle[] GetCircleArray(string name)
        {
            _tools.GetCircleFArray(name, out double[] valuesX, out double[] valuesY, out double[] valuesR);
            var num = valuesX.Length;
            var values = new Circle[num];
            for (int i = 0; i < num; i++)
            {
                var p1 = new Point(valuesX[i], valuesY[i]);
                values[i] = new Circle(p1, valuesR[i]);
            }
            return values;
        }

        public Ellipse[] GetEllipseArray(string name)
        {
            _tools.GetEllipseFArray(name, out double[] valuesX, out double[] valuesY, out double[] valuesR1, out double[] valuesR2, out double[] valuesA);
            var num = valuesX.Length;
            var values = new Ellipse[num];
            for (int i = 0; i < num; i++)
            {
                var p1 = new Point(valuesX[i], valuesY[i]);
                values[i] = new Ellipse(p1, valuesR1[i], valuesR2[i], valuesA[i]);
            }
            return values;
        }

        public Line[] GetLineArray(string name)
        {
            _tools.GetLineFArray(name, out double[] valuesX1, out double[] valuesY1, out double[] valuesX2, out double[] valuesY2);
            var num = valuesX1.Length;
            var values = new Line[num];
            for (int i = 0; i < num; i++)
            {
                var p1 = new Point(valuesX1[i], valuesY1[i]);
                var p2 = new Point(valuesX2[i], valuesY2[i]);
                values[i] = new Line(p1, p2);
            }
            return values;
        }
    }
}
