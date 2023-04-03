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

        public static void FillByImage(int imgIndex, int size)
        {
            //Image image = new Image();
            Texture2D icon = listTexture[imgIndex];
            Image image = icon.GetImage();
            if (size > maxXY)
                size = maxXY;

            int imgWidth = image.GetWidth();
            int imgHeight = image.GetHeight();

            int addXY = (maxXY - imgWidth) / 2;
            int addZ = (maxZ - imgHeight) / 2;

            int addSXY = (maxXY - size) / 2;
            //int addSY = (maxZ - size) / 2;

            Godot.Color[,] imgCopy = new Godot.Color[maxZ, maxXY];
            for (int y = 0; y < imgHeight; y++)
                for (int x = 0; x < imgWidth; x++)
                {
                    imgCopy[y + addZ, x + addXY] = image.GetPixel(x, y);
                    //GD.Print(x + "+" + y);
                }
            int Xmin = addXY;
            if (Xmin < 0) Xmin = 0;
            int Xmax = imgWidth + addXY;
            if (Xmax > maxXY) Xmax = maxXY;

            int Ymin = addSXY;
            if (Ymin < 0) Ymin = 0;
            int Ymax = size + addSXY;
            if (Ymax > maxXY) Ymax = maxXY;

            for (int x = Xmin; x < Xmax; x++)
                for (int y = Ymin; y < Ymax; y++)
                    for (int z = 0; z < maxZ; z++)
                    {
                        //if (imgCopy[z, x] == new Godot.Color(0.5019608f, 0.5019608f, 0.5019608f, 1))
                        //    imgCopy[z, x].A = 0;
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
            while (rotation < 0)
                rotation += 360;
            while (rotation > 360)
                rotation -= 360;
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
            if (rotation < 180)
            {
                minX = maxXY-1;
                maxX = -1;
                addX = -1;
            }
            if ((rotation > 90) && (rotation < 270))
            {
                minY = maxXY - 1;
                maxY = -1;
                addY = -1;
            }
            
            double tempAddCenter = ((maxXY - 1) * 0.5 * zoom);

            for (int x = minX; x != maxX; x += addX)
                for (int y = minY; y != maxY; y += addY)
                {
                    int newX = (int)(((x - tempAddCenter) * c - (y - tempAddCenter) * s) + tempAddCenter + 0.0001);
                    int newY = (int)(((x - tempAddCenter) * s + (y - tempAddCenter) * c) + tempAddCenter + 0.0001);
                    if ((newX < 0) || (newY < 0))
                        continue;
                    if ((newX > maxXY - 1) || (newY > maxXY - 1))
                        continue;
                    //int tempIndex = newX * maxXYZ + newY * maxZ;
                    for (int z = 0; z < maxZ; z++)
                    {
                        if (voxArray[z, newY, newX].A == 1)
                        {
                            imgCopy[z, x] = voxArray[z, newY, newX];
                        }
                    }
                }
            return imgCopy;
        }

        internal static Vector2 GetSize()
        {
            return new Vector2(maxXY, maxZ);
        }
        /*
.(0st)
123
804
765
↑
765,20 21 22
Y+ X+-

.(45st)
123
804
765
↗
18765
Y+ X-,20 21 12 00 22 

.(90st)
123
804
765
→
187
Y+- X-,00 10 20

.(135st)
123
804
765
↘
32187
Y- X-,00 01 10 02 20

.(180st)
123
804
765
↓
321
Y- X+-,02 01 00

.(225st)
123
804
765
↙
54321
Y- X+,02 01 12 00 22

.(270st)
123
804
765
←
543
Y+- X+,22 21 20

.(315st)
123
804
765
↖
76543,22 21 12 02 20
Y+ X+ 
*/
    }
}
