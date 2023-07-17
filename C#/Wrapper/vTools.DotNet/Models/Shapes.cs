
using System;

namespace vTools.DotNet.Models
{
    public struct Point
    {
        public Point(double x = 0, double y = 0)
        {
            X = x; Y = y;
        }
        public double X { get; set; }
        public double Y {  get; set; }
    }
    public struct Size
    {
        public Size(double w = 0, double h = 0)
        {
            Width = w; Height = h;
        }
        public double Width { get; set; }
        public double Height { get; set; }
    }
    public struct Rectangle
    {
        public Rectangle(Point p = new Point(), Size s = new Size(), double a = 0)
        {
            Point = p;
            Size = s;
            Angle = a;
        }
        public Point Point { get; set; }
        public Size Size { get; set; }
        public double Angle { get; set; }
    }
    public struct Circle
    {
        public Circle(Point p = new Point(), double r = 0)
        {
            Center = p;
            Radius = r;
        }
        public Point Center { get; set; }
        public double Radius { get; set; }
    }
    public struct Ellipse
    {
        public Ellipse(Point p = new Point(), double r1 = 0, double r2 = 0, double a = 0)
        {
            Center = p;
            Radius1 = r1;
            Radius2 = r2;
            Angle = a;
        }
        public Point Center { get; set; }
        public double Radius1 { get; set; }
        public double Radius2 { get; set; }
        public double Angle { get; }
    }
    public struct Line
    {
        /// <summary>
        /// First point on top and left. <br />
        /// Sencond point on bottom and right. <br />
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        public Line(Point p1 = new Point(), Point p2 = new Point())
        {
            Point1 = p1;
            Point2 = p2;
            Width = Math.Abs(p1.X - p2.X);
            Height = Math.Abs(p1.Y - p2.Y);
        }
        public Point Point1 { get; set; }
        public Point Point2 { get; set; }
        public double Top => Point1.Y;
        public double Left => Point1.X;
        public double Bottom => Point2.Y;
        public double Right => Point2.X;
        public double Width { get; }
        public double Height { get; }
    }
}
