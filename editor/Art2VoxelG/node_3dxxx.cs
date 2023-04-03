using Godot;
using System;

public partial class node_3dxxx : Node3D
{
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
        // Vytvoøení nové instance BoxMesh.
        var boxMesh = new BoxMesh();

        // Vytvoøení nové instance SpatialMaterial s nastaveným zeleným barvou.
        var material = new StandardMaterial3D();
        material.AlbedoColor = Colors.Green;

        // Vytvoøení nové instance MeshInstance a nastavení BoxMesh a SpatialMaterial.
        var meshInstance = new Godot.MeshInstance3D();
        meshInstance.Mesh = boxMesh;
        meshInstance.MaterialOverlay = material;

        // Pøidání MeshInstance do scény.
        AddChild(meshInstance);

        //AddChild(new Node3DVoxel());
        //GetViewport().ClearColor = new Color(1, 1, 1);
        /*
		 Material material = new SpatialMaterial();
material.AlbedoColor = new Color(1, 1, 1);
material.ParamsAmbientLight = 0;
material.ParamsDiffuseLight = 0;
material.ParamsSpecularLight = 0;
		*/
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
	{
	}
}
