#ifndef LIFE_HH
#define LIFE_HH

#include <cstddef>
#include <cstring>

struct RGB
{
	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;
};

void count_cell (RGB& dstcell,
                 RGB& srccell)
{
	if (srccell.red)   ++dstcell.red;
	if (srccell.green) ++dstcell.green;
	if (srccell.blue)  ++dstcell.blue;
}

template <std::size_t N>
void count_row (RGB (&dstrow) [N],
                RGB (&srcrow) [N])
{
	count_cell (dstrow [N - 1], srcrow [0]);
	count_cell (dstrow [0],     srcrow [0]);
	count_cell (dstrow [1],     srcrow [0]);

	for (int col = 1; col < N - 1; ++col) {
		count_cell (dstrow [col - 1], srcrow [col]);
		count_cell (dstrow [col],     srcrow [col]);
		count_cell (dstrow [col + 1], srcrow [col]);
	}

	count_cell (dstrow [N - 2], srcrow [N - 1]);
	count_cell (dstrow [N - 1], srcrow [N - 1]);
	count_cell (dstrow [0],     srcrow [N - 1]);
}

void step_cell (RGB& dstcell,
                RGB& srccell)
{
	auto do_color = [&] (std::uint8_t (RGB::*color)) {
		if (srccell.*color)
			dstcell.*color = (dstcell.*color == 3 || dstcell.*color == 4) ? 0xFF : 0x00;
		else
			dstcell.*color = (dstcell.*color == 3) ? 0xFF : 0x00;
	};

	do_color (&RGB::red);
	do_color (&RGB::green);
	do_color (&RGB::blue);
}

template <std::size_t N>
void step_life (RGB (&dststate) [N] [N],
                RGB (&srcstate) [N] [N])
{
	std::memset (&dststate, 0, sizeof (dststate));

	count_row (dststate [N - 1], srcstate [0]);
	count_row (dststate [0],     srcstate [0]);
	count_row (dststate [1],     srcstate [0]);

	for (int row = 1; row < N - 1; ++row) {
		count_row (dststate [row - 1], srcstate [row]);
		count_row (dststate [row],     srcstate [row]);
		count_row (dststate [row + 1], srcstate [row]);
	}

	count_row (dststate [N - 2], srcstate [N - 1]);
	count_row (dststate [N - 1], srcstate [N - 1]);
	count_row (dststate [0],     srcstate [N - 1]);

	for (int row = 0; row < N; ++row)
		for (int col = 0; col < N; ++col)
			step_cell (dststate [row] [col],
			           srcstate [row] [col]);
}

#endif
