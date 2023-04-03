using Art2Voxel;
using Godot;
using System;
using System.IO;
//using System.IO; //for Get directory

public partial class node_2d : Node2D
{
	// Called when the node enters the scene tree for the first time.
    public override void _Ready()
	{
        VoxelClass.Init();
        VoxelClass.AnalyzeFolder("res://img/");
        VoxelClass.CreateVoxelArray();
        VoxelClass.FillByImage(0,1);
        VoxelClass.inited=true;
        TextureRect textNode = (TextureRect)GetNode("Window/SubViewportContainer/SubViewport/MyTextureRect");
        textNode.UpdatePos();
        TextureRect textNode2 = (TextureRect)GetNode("Window2/SubViewportContainer/SubViewport/MyTextureRect");
        textNode2.UpdatePos();
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
	{
	}
}
