using Godot;
using System;

public partial class node_3d : Node3D
{
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
        for (int z = 0; z < 1; z++)
            for (int y = 0; y < 100; y++)
            for (int x=0;x<100;x++)
                AddBox(x,y,0,1/100f*x, 1 / 100f * y, 0);
    }

    private void AddBox(float posX, float posY, float posZ, float r, float g, float b)
    {
        var boxMesh = new BoxMesh();

        var material = new StandardMaterial3D();
        material.AlbedoColor = new Godot.Color(r, g, b);

        var meshInstance = new Godot.MeshInstance3D();
        meshInstance.Mesh = boxMesh;
        meshInstance.MaterialOverlay = material;
        meshInstance.Position = new Vector3(posX, posY, posZ);
        AddChild(meshInstance);
    }

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}
}
