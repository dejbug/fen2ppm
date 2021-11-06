#pragma once
#include <algorithm> // max

struct grid_t
{
	size_t cols = 0;
	size_t rows = 0;
	size_t edge = 0;
	size_t gap = 0;

	void set_size(size_t cols, size_t rows)
	{
		this->cols = cols;
		this->rows = rows;
	}

	void set_square_size(size_t edge, size_t gap=0)
	{
		this->edge = edge;
		this->gap = gap;
	}

	void get_square_rect(RECT & r, size_t col, size_t row) const
	{
		r.left = col * (edge + gap) + gap;
		r.top = row * (edge + gap) + gap;
		r.right = r.left + edge;
		r.bottom = r.top + edge;
	}
	
	void get_bounds(RECT & r) const
	{
		r.left = r.top = 0;
		r.right = r.left + cols * (edge + gap) + gap;
		r.bottom = r.top + rows * (edge + gap) + gap;
	}
	
	size_t get_image_edge() const
	{
		RECT r;
		get_bounds(r);
		return std::max(r.right - r.left, r.bottom - r.top);
	}
};


// void draw(HDC dc, grid_t const & grid)
// {
	// HGDIOBJ const oldBrush = SelectObject(dc, GetStockObject(DC_BRUSH));
	// HGDIOBJ const oldPen = SelectObject(dc, GetStockObject(DC_PEN));
	// SetDCPenColor(dc, RGB(160,100,60));
	// COLORREF lt = RGB(210,200,200);
	// COLORREF dk = RGB(210,1,100);
	// for (size_t row=0; row<grid.rows; ++row)
	// {
		// for (size_t col=0; col<grid.cols; ++col)
		// {
			// RECT r;
			// grid.get_square_rect(r, col, row);
			// SetDCBrushColor(dc, ((row + col) % 2 == 0) ? lt : dk);
			// Rectangle(dc, r.left, r.top, r.right, r.bottom);
			// printf("%d %d %d\n", col, row, (col + row) % 2);
		// }
	// }
	// SelectObject(dc, oldPen);
	// SelectObject(dc, oldBrush);
// }
