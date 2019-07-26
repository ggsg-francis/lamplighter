#pragma once
namespace index
{
	void Init();
	void End();
	void Tick(btf32 DELTA);
	void Draw();

	void SetViewFocus(btID i);
	fw::Vector2 GetViewOffset();
}