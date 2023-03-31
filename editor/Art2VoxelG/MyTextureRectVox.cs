using Art2Voxel;
using Godot;
using System;

public partial class MyTextureRectVox : TextureRect
{
    private bool first_run = true;
    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
	{
	}

    public void StartRun()
    {
        if (first_run)
        {
            if (VoxelClass.inited)
            {
                first_run = false;
                InitImage();
            }
        }
    }

    public void InitImage()
    {
        ImageTexture texture = new ImageTexture();
        Godot.Image image = Godot.Image.Create(VoxelClass.maxXY, VoxelClass.maxZ, false, Godot.Image.Format.Rgba8);
        //for (int i = 0; i < 50; i++)
        //    image.SetPixel(i, i, new Godot.Color(255, 0, 0, 255));
        texture = ImageTexture.CreateFromImage(image);
        Texture = texture;
        TextureFilter = TextureFilterEnum.Nearest;
    }

    public void DrawVoxel()
    {
        Godot.Color[,] imgCopy = VoxelClass.GetImage(0);
        Image image = Texture.GetImage();

        for (int x = 0; x < image.GetWidth(); x++)
            for (int y = 0; y < image.GetHeight(); y++)
                image.SetPixel(x, y, imgCopy[x, y]);
                //image.SetPixel(x, y, new Godot.Color(255, 0, 0, 255));
        Texture = ImageTexture.CreateFromImage(image);
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
	{
        StartRun();
        DrawVoxel();
    }
}
