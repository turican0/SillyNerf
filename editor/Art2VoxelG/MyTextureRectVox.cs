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
        for (int i = 0; i < 50; i++)
            image.SetPixel(i, i, new Godot.Color(255, 0, 0, 255));
        texture = ImageTexture.CreateFromImage(image);
        Texture = texture;
    }

    public void DrawVoxel()
    {
        /*for(int i=0;i<50;i++)
            Texture.GetImage().SetPixel(i,i, new Godot.Color(255,0,0,255));*/
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
	{
        StartRun();
        DrawVoxel();
    }
}
