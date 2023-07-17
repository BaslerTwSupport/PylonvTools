using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace vTools.DotNet.Models
{
    public struct Img
    {
        public Img(byte[] bytes = null, int width = 0, int height = 0, int channels = 1)
        {
            Bytes = bytes;
            Width = width;
            Height = height;
            Channels = channels;
        }
        public byte[] Bytes { get; }
        public int Width { get; }
        public int Height { get; }
        public int Channels { get; }
    }
}
