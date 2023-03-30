using Godot;
using System;

public partial class VSlider1 : VSlider
{
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}
	private void _on_v_slider_1te_text_changed()
	{
        TextEdit _myChildNode = GetNode<TextEdit>("VSlider1TE");
        if (float.TryParse(_myChildNode.Text, out float value))
        {
            Value = value;
            _myChildNode.Text = value.ToString();
        }
    }
}



