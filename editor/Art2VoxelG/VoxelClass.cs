using Godot;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Godot.HttpRequest;

namespace Art2Voxel
{
    internal static class VoxelClass
    {
        public static int maxXY = 0;
        public static int maxZ = 0;
        public static bool inited = false;
        static Godot.Color[,,] voxArray;
        static List<Texture2D> listTexture;

        public static void Init()
        {
            listTexture = new List<Texture2D>();
        }
        public static Vector2 GetPNGSize(string filePath)
        {
            Image image = new Image();
            Texture2D icon = ResourceLoader.Load(filePath) as Texture2D;
            listTexture.Add(icon);
            Vector2 result;
            result.X = icon.GetWidth();
            result.Y = icon.GetHeight();
            return result;
        }
        public static void AnalyzeImage(string file)
        {
            Vector2 pngSize = GetPNGSize(file);
            if (pngSize.X > maxXY)
                maxXY = (int)pngSize.X;
            if (pngSize.Y > maxZ)
                maxZ = (int)pngSize.Y;
        }

        public static void CreateVoxelArray()
        {
            voxArray = new Godot.Color[maxXY, maxXY, maxZ];
        }

        public static void FillByImage(int imgIndex,int size)
        {
            //Image image = new Image();
            Texture2D icon = listTexture[imgIndex];
            Image image = icon.GetImage();
            if (size > maxXY)
                size = maxXY;

            Godot.Color[,] imgCopy = new Godot.Color[maxXY, maxZ];
            for (int x = 0; x < maxXY; x++)
                for (int y = 0; y < maxZ; y++)
                        imgCopy[x, y] = image.GetPixel(x, y);
            for (int x = 0; x < maxXY; x++)
                for (int y = 0; y < size; y++)
                    for (int z = 0; z < maxZ; z++)
                    {
                        voxArray[x, y, z] = imgCopy[x, y];
                        //voxArray[x, y, z] = image.GetPixel(x,z);
                        //voxArray[x, y, z].R = imgData[(x + z * image.GetWidth())*2 + 0] / (float)256;// image.GetPixel(x,z);
                        //voxArray[x, y, z].G = imgData[(x + z * image.GetWidth()) * 4 + 1] / (float)256;// image.GetPixel(x,z);
                        //voxArray[x, y, z].B = imgData[(x + z * image.GetWidth()) * 4 + 2] / (float)256;// image.GetPixel(x,z);
                        //voxArray[x, y, z].A = imgData[(x + z * image.GetWidth()) * 4 + 3] / (float)256;// image.GetPixel(x,z);
                    }
        }

        static public void AnalyzeFolder(string directoryPath)
        {
            //string[] files = Directory.GetFiles(directoryPath);
            string[] files = Directory.GetFiles(ProjectSettings.GlobalizePath(directoryPath), "*.png");
            foreach (string file in files)
            {
                VoxelClass.AnalyzeImage(file);
            }
        }

    }
}
