using Godot;
using System;

//[Tool]
public partial class MyTextureRect : Godot.TextureRect
{
    public bool first_run = true;
    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {      
        StartRun();
    }

    private void StartRun()
    {
        if (first_run)
        {
            first_run = false;
            LoadImageAsTexture("res://TMAPS2-0-000-00.pngGr.png");
            SetGrid();
        }
    }
    private void SetGrid()
    {
        TextureRect childGridMode = new TextureRect();
        // Získání velikosti TextureRect
        Vector2 rectSize = this.Texture.GetSize();

        // Vytvoření nového Image s bílým pozadím, odpovídající velikosti TextureRect
        //Image gridImage = Image.Create((int)rectSize.X, (int)rectSize.Y, false, Image.Format.Rgba8);
        //childnode.Texture = new Texture();
        // Nastavení barvy čar na šedou
        Color lineColor = new Color(0.5f, 0.5f, 0.5f);

        // Nakreslení vodorovných čar
        for (int y = 0; y < rectSize.Y; y++)
        {
            childGridMode.DrawLine(new Vector2(0, y), new Vector2(rectSize.X, y), lineColor);
        }

        // Nakreslení svislých čar
        ////for (int x = 0; x < rectSize.X; x += 1)
        ////{
        ////    childGridMode.DrawLine(new Vector2(x, 0), new Vector2(x, rectSize.Y), lineColor);
        ////}

        // Vytvoření nového Texture z Image
        //Texture gridTexture = new Texture();
        //gridTexture.CreateFromImage(gridImage);

        // Nastavení nového Texture na TextureRect
        //textureRect.Texture = gridTexture;
        AddChild(childGridMode);
    }

    private void LoadImageAsTexture(string imagePath)
    {
        Texture2D icon = ResourceLoader.Load(imagePath) as Texture2D;

        this.Texture = icon;

    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
    {
            StartRun();
    }

    public override void _Input(InputEvent inputEvent)
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
