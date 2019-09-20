#pragma once
namespace index
{
	void Init();
	void End();
	void Tick(btf32 DELTA);
	void TickGUI();
	void Draw(bool oob = true);
	void DrawGUI();
	// For drawing debug GUI on top of the game framebuffer
	void DrawPostDraw();

	void SetViewFocus(btID i);
	m::Vector2 GetViewOffset();

	void SetShadowTexture(btui32 ID);
}