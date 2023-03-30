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
        VoxelClass.FillByImage(0,30);
        VoxelClass.inited=true;
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
	{
	}
}
