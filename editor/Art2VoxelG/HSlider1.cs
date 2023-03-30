using Godot;
using System;
using static System.Net.Mime.MediaTypeNames;

public partial class HSlider1 : HSlider
{
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}

	private void _on_h_slider_1te_text_changed()
	{
		TextEdit _myChildNode = GetNode<TextEdit>("HSlider1TE");
		if (float.TryParse(_myChildNode.Text, out float value))
		{
			Value = value;
			_myChildNode.Text = value.ToString();
		}
	}

}

