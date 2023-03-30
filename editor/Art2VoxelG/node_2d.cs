using Art2Voxel;
using Godot;
using System;
using System.IO;
//using System.IO; //for Get directory

public partial class node_2d : Node2D
{
	// Called when the node enters the scene tree for the first time.
	VoxelClass voxelClass;

    public override void _Ready()
	{
		voxelClass = new VoxelClass();
		AnalyzeFolder("res://img/");
        voxelClass.CreateVoxelArray();
    }

	private void AnalyzeFolder(string directoryPath)
	{
        //string[] files = Directory.GetFiles(directoryPath);
        string[] files = Directory.GetFiles(ProjectSettings.GlobalizePath(directoryPath), "*.png");
        foreach (string file in files)
        {
            voxelClass.AnalyzeImage(file);
        }
    }	

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
	{
	}
}
