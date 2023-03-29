using Godot;
using System;
using System.Drawing;
using System.Security.Cryptography;
using static Godot.RenderingServer;
using static System.Net.Mime.MediaTypeNames;

[Tool]
public partial class TextureRect : Godot.TextureRect{
    public bool first_run = true;
    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {      
        StartRun();
    }

    private void StartRun()
    {
        if(Name== "MyTextureRect")
            if (first_run)
            {
                first_run = false;
                LoadImageAsTexture("res://TMAPS2-0-000-00.pngGr.png");
                //loadB();
                SetGrid();
                var node2D = new Node2D();
                AddChild(node2D);
            }
    }

    /*public void loadB()
    {
        // Vytvoření nové textury 100x100
        ImageTexture texture = new ImageTexture();
        Godot.Image image = Godot.Image.Create(100, 100, false, Godot.Image.Format.Rgb8);
        // Nakreslení šikmé čáry
        var color = new Godot.Color(255, 255, 255);
        for (int i = 0; i < 100; i++)
        {
            image.SetPixel(i, i, color);
        }
        texture = ImageTexture.CreateFromImage(image);
        // Použití textury jako zdroje pro zobrazení TextureRect
        Texture = texture;
    }*/
    private void SetGrid()
    {
        int scale = 10;
        TextureRect childGridMode = new TextureRect();
        childGridMode.Name = "MyGridTextureRect";
        Vector2 currentScale = childGridMode.Scale;
        currentScale.X = 1.0f/ scale;
        currentScale.Y = 1.0f/ scale;
        childGridMode.Scale = currentScale;
        Vector2 rectSize = Texture.GetSize();

        // Vytvoření nové textury 100x100
        ImageTexture texture = new ImageTexture();
        Godot.Image image = Godot.Image.Create((int)(rectSize.X* scale), (int)(rectSize.Y* scale), false, Godot.Image.Format.Rgba8);
        // Nakreslení šikmé čáry
        var color = new Godot.Color(255, 255, 255, 255);
        /*for (int i = 0; i < 100; i++)
        {
            image.SetPixel(i, i, color);
        }*/
        for (int y = 0; y < rectSize.Y* scale; y++)
            for (int x = 0; x < rectSize.X* scale; x += scale)
                image.SetPixel(x, y, new Godot.Color(0, 0, 0, 255));
        for (int y = 0; y < rectSize.Y* scale; y += scale)
            for (int x = 0; x < rectSize.X* scale; x++)
                image.SetPixel(x, y, new Godot.Color(0, 0, 0, 255));
        texture = ImageTexture.CreateFromImage(image);
        // Použití textury jako zdroje pro zobrazení TextureRect
        childGridMode.Texture = texture;


        /*
        var texture = new ImageTexture();
        var gridImage = Godot.Image.Create((int)rectSize.X, (int)rectSize.Y, false, Godot.Image.Format.Rgba8);
        //childGridMode.Texture = texture;

        //gridImage.lock();
        for (int y = 0; y < rectSize.Y; y++)
            for (int x = 0; x < rectSize.X; x+=10)
                gridImage.SetPixel(x, y, new Godot.Color(0,0,0,255));
        for (int y = 0; y < rectSize.Y; y+=10)
            for (int x = 0; x < rectSize.X; x++)
                gridImage.SetPixel(x, y, new Godot.Color(0, 0, 0,255));

        texture = ImageTexture.CreateFromImage(gridImage);
        //gridImage.unlock();
        */

        AddChild(childGridMode);
    }

    private void LoadImageAsTexture(string imagePath)
    {
        Texture2D icon = ResourceLoader.Load(imagePath) as Texture2D;

        Texture = icon;

    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
    {
            StartRun();
    }

    public override void _Input(InputEvent inputEvent)
    {
        if (Name == "MyTextureRect")
        {
            if (inputEvent is InputEventMouseButton mouseButtonEvent)
            {
                if (mouseButtonEvent.Pressed && mouseButtonEvent.ButtonIndex == MouseButton.Left)
                {
                    GD.Print("Left mouse button pressed");
                }
                else if (mouseButtonEvent.Pressed && mouseButtonEvent.ButtonIndex == MouseButton.Right)
                {
                    GD.Print("Right mouse button pressed");
                }
                else if (mouseButtonEvent.Pressed && mouseButtonEvent.ButtonIndex == MouseButton.WheelUp)
                {
                    Vector2 currentScale = this.Scale;
                    currentScale.X *= 1.1f;
                    currentScale.Y *= 1.1f;
                    this.Scale = currentScale;
                }
                else if (mouseButtonEvent.Pressed && mouseButtonEvent.ButtonIndex == MouseButton.WheelDown)
                {
                    Vector2 currentScale = this.Scale;
                    currentScale.X *= 0.9f;
                    currentScale.Y *= 0.9f;
                    this.Scale = currentScale;
                }
            }
            else if (inputEvent is InputEventMouseMotion mouseMotionEvent)
            {
                if (mouseMotionEvent.Relative != Vector2.Zero)
                {
                    GD.Print("Mouse moved");
                }
            }
        }
    }
}
