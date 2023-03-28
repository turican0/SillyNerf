using Godot;
using System;
using System.ComponentModel.Design;

[Tool]
public partial class Sprite2D : Godot.Sprite2D
{
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
        LoadImageAsTexture("res://TMAPS2-0-000-00.pngGr.png");
    }

    private void LoadImageAsTexture(string imagePath)
    {
        Texture2D icon = ResourceLoader.Load(imagePath) as Texture2D;

        this.Texture = icon;        
        /*
        var material = new StandardMaterial3D();
        material.AlbedoTexture = icon;
        a_mesh.SurfaceSetMaterial(0, material);

        this.Mesh = a_mesh;

        var image = new Image();
        var error = image.Load(imagePath);

        if (error == Error.Ok)
        {
            var texture = new ImageTexture();
            texture..CreateFromImage(image);
            this.Texture = texture;
        }
        else
        {
            GD.Print($"Error loading image: {error}");
        }*/
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
	{
	}
}
