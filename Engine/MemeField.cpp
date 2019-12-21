#include "MemeField.h"
#include <assert.h>
#include <random>
#include "Vei2.h"
#include "SpriteCodex.h"
#include <algorithm>

void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

void MemeField::Tile::Reveal()
{	
	assert(state == State::Hidden);
	state = State::Revealed;
}

void MemeField::Tile::ToggleFlag()
{
	assert( !IsRevealed() );
	if (state == State::Hidden)
	{
		state = State::Flagged;
	}
	else 
	{
		state = State::Hidden;
	}
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw(const Vei2& screenPos, bool fucked, bool win, Graphics& gfx) const
{
	if (!fucked) {
		if(win)
		{
			SpriteCodex::DrawWin(gfx);
		}
		switch (state)
		{
		case State::Hidden:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			break;
		case State::Flagged:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			SpriteCodex::DrawTileFlag(screenPos, gfx);
			break;
		case State::Revealed:
			if (!HasMeme())
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			break;
		}
	}
	else // we are fucked
	{
		switch (state)
		{
		case State::Hidden:
			if (HasMeme())
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
			}
			break;
		case State::Flagged:
			if (HasMeme())
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else {
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}			
			break;
		case State::Revealed:
			if (!HasMeme())
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			break;
		}
	}
}

void MemeField::Tile::SetNeighborMemeCount(int memeCount)
{
	assert(nNeighborMemes == -1);
	nNeighborMemes = memeCount;
}

MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width * height);
	nMemesCount = nMemes;

	MemeFieldPos = {
		Graphics::GetScreenRect().GetCenter().x - (width * SpriteCodex::tileSize) / 2,
		Graphics::GetScreenRect().GetCenter().y - (height * SpriteCodex::tileSize) / 2
	};

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int nSpawned = 0; nSpawned < nMemes; ++nSpawned)
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist(rng), yDist(rng) };
		} while (TileAt(spawnPos).HasMeme());
		TileAt(spawnPos).SpawnMeme();
	}

	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).SetNeighborMemeCount(CountNeighborMemes(gridPos));
		}
	}
}

void MemeField::Draw(Graphics& gfx) const
{
	const RectI rect = GetRect();

	// Draw border
	const int top = rect.top - borderWidth;
	const int left = rect.left - borderWidth;
	const int bottom = top + borderWidth * 2 + height * SpriteCodex::tileSize;
	const int right = left + borderWidth * 2 + width * SpriteCodex::tileSize;

	// top
	gfx.DrawRect(left, top, right, top + borderWidth, borderColor);
	// left
	gfx.DrawRect(left, top + borderWidth, left + borderWidth, bottom - borderWidth, borderColor);
	// right
	gfx.DrawRect(right - borderWidth, top + borderWidth, right, bottom - borderWidth, borderColor);
	// bottom
	gfx.DrawRect(left, bottom - borderWidth, right, bottom, borderColor);

	// Draw background
	gfx.DrawRect(rect, SpriteCodex::baseColor);

	// Draw cell
	for (Vei2 gridPos = {0,0}; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).Draw(gridPos * SpriteCodex::tileSize + MemeFieldPos, isFucked, isGameWin, gfx);
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI(MemeFieldPos.x, width * SpriteCodex::tileSize + MemeFieldPos.x, MemeFieldPos.y, height * SpriteCodex::tileSize + MemeFieldPos.y);
}

int MemeField::GetMemeCount() const
{
	return nMemesCount;
}

void MemeField::OnRevealClick(const Vei2 screenPos)
{
	if (!isFucked && !isGameWin)
	{
		const Vei2 gridPos = ScreenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);

		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed() && !tile.IsFlagged())
		{
			tile.Reveal();
			if (tile.HasMeme())
			{
				isFucked = true;
			}
		}
	}	
}

void MemeField::OnFlagClick(const Vei2 screenPos)
{
	if (!isFucked && !isGameWin)
	{
		const Vei2 gridPos = ScreenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);

		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed())
		{
			tile.ToggleFlag();
		}
		int countRight = 0;
		for (const Tile tile : field)
		{
			if (tile.HasMeme() && tile.IsFlagged())
			{
				countRight++;
			}
		}
		if (countRight == GetMemeCount())
		{
			isGameWin = true;
		}
	}
}

MemeField::Tile& MemeField::TileAt(const Vei2& gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

const MemeField::Tile& MemeField::TileAt(const Vei2& gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2 MemeField::ScreenToGrid(const Vei2 screenPos) const
{
	return (screenPos - MemeFieldPos) / SpriteCodex::tileSize;
}

int MemeField::CountNeighborMemes(const Vei2& gridPos)
{
	const int xStart = std::max(0, gridPos.x - 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int xEnd = std::min(width - 1, gridPos.x + 1);
	const int yEnd = std::min(height - 1, gridPos.y + 1);

	int count = 0;
	for (Vei2 gridPos = {xStart,yStart}; gridPos.y <= yEnd; gridPos.y++)
	{
		for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
		{
			if (TileAt(gridPos).HasMeme())
			{
				count++;
			}
		}
	}

	return count;
}
