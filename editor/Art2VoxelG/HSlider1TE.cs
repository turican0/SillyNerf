using Godot;
using System;

public partial class HSlider1TE : TextEdit
{
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		Text = "50";

	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}

	private void _on_h_slider_1_value_changed(double value)
	{
		Text = value.ToString();
	}

}

