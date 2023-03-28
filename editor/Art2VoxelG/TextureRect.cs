using Godot;
using System;

public partial class TextureRect : Godot.TextureRect
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

    }
    
    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
	{
	}
}
