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
            voxArray = new Godot.Color[maxZ, maxXY, maxXY];
        }

        public static void FillByImage(int imgIndex,int size)
        {
            //Image image = new Image();
            Texture2D icon = listTexture[imgIndex];
            Image image = icon.GetImage();
            if (size > maxXY)
                size = maxXY;

            Godot.Color[,] imgCopy = new Godot.Color[maxXY, maxZ];
            for (int y = 0; y < maxZ; y++)
                for (int x = 0; x < maxXY; x++)
                        imgCopy[y, x] = image.GetPixel(x, y);
            for (int x = 0; x < maxXY; x++)
                for (int y = 0; y < size; y++)
                    for (int z = 0; z < maxZ; z++)
                    {
                        if (imgCopy[z, x] == new Godot.Color(0.5019608f, 0.5019608f, 0.5019608f, 1))
                            imgCopy[z, x].A = 0;
                        voxArray[z, y, x] = imgCopy[z, x];
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

        internal static Color[,] GetImage(double rotation)
        {
            Godot.Color[,] imgCopy = new Godot.Color[maxZ, maxXY];
            double angle = Math.PI * rotation / 180.0;
            double s = Math.Sin(angle);
            double c = Math.Cos(angle);

            int maxXYZ = maxXY * maxZ;
            int zoom = 1;
            int minX = 0;
            int maxX = maxXY;
            int addX = 1;
            int minY = 0;
            int maxY = maxXY;
            int addY = 1;

            double tempAddCenter = ((maxXY - 1) * 0.5 * zoom);

            for (int x = minX; x != maxX; x += addX)
                for (int y = minY; y != maxY; y += addY)
                {
                    int newX = (int)(((x - tempAddCenter) * c - (y - tempAddCenter) * s) + tempAddCenter + 0.01);
                    int newY = (int)(((x - tempAddCenter) * s + (y - tempAddCenter) * c) + tempAddCenter + 0.01);
                    if ((newX < 0) || (newY < 0))
                        continue;
                    if ((newX > maxXY - 1) || (newY > maxXY - 1))
                        continue;
                    //int tempIndex = newX * maxXYZ + newY * maxZ;
                    for (int z = 0; z < maxZ; z++)
                    {
                        if (voxArray[z, y, x].A == 1)
                        {
                            imgCopy[z, x] = voxArray[z, newY, newX];
                        }
                    }
                }
            return imgCopy;
        }
    }
}
