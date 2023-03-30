using Godot;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Godot.HttpRequest;

namespace Art2Voxel
{
    internal class VoxelClass
    {
        int maxX = 0;
        int maxY = 0;

        public Vector2 GetPNGSize(string filePath)
        {
            Image image = new Image();
            Texture2D icon = ResourceLoader.Load(filePath) as Texture2D;
            Vector2 result;
            result.X = icon.GetWidth();
            result.Y = icon.GetHeight();
            return result;
        }
        public void AnalyzeImage(string file)
        {
            Vector2 pngSize = GetPNGSize(file);
            if (pngSize.X > maxX)
                maxX = (int)pngSize.X;
            if (pngSize.Y > maxY)
                maxY = (int)pngSize.Y;
        }

        public void CreateVoxelArray()
        {
        }
    }
}
