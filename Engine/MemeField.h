#pragma once
#include "Graphics.h"
#include "RectI.h"

class MemeField
{

private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		void SpawnMeme();
		void Reveal();
		void ToggleFlag();
		bool IsRevealed() const;
		bool IsFlagged() const;
		bool HasMeme() const;
		void Draw(const Vei2& screenPos, Graphics& gfx) const;
	private:
		State state = State::Hidden;
		bool hasMeme = false;
	};
public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void OnRevealClick(const Vei2 screenPos);
	void OnFlagClick(const Vei2 screenPos);
private:
	Tile& TileAt(const class Vei2& gridPos);
	const Tile& TileAt(const class Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2 screenPos) const;
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width * height];
};

