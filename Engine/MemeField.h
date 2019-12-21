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
		void Draw(const Vei2& screenPos, bool fucked, bool win, Graphics& gfx) const;
		void SetNeighborMemeCount(int memeCount);
	private:
		State state = State::Hidden;
		bool hasMeme = false;
		int nNeighborMemes = -1;
	};
public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	int GetMemeCount() const;
	void OnRevealClick(const Vei2 screenPos);
	void OnFlagClick(const Vei2 screenPos);
private:
	static constexpr int borderWidth = 8;
	static constexpr Color borderColor = Colors::Blue;
	Vei2 MemeFieldPos;
	Tile& TileAt(const class Vei2& gridPos);
	const Tile& TileAt(const class Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2 screenPos) const;
	int CountNeighborMemes(const Vei2& gridPos);
	int nMemesCount = 0;
private:
	static constexpr int width = 6;
	static constexpr int height = 4;
	bool isFucked = false;
	bool isGameWin = false;
	Tile field[width * height];
};

