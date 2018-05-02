//seam.cpp
//michael fiander

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <vector>

int find_smallest(int, int);
int find_smallest(int, int, int);
void trace_seam(int, int, std::vector<int>&, std::vector<std::vector<int> >&);
void remove_seam(int&, int&, std::vector<std::vector<int> >&);

int main(int argc, char* argv[]) {

	if(argc < 3) {
		std::cout << "You have provided too few arguements. Terminating program.\n";
		return 0;
	}

	std::string filename = argv[0];
	int x_seams = *argv[1];
	int y_seams = *argv[2];

	std::ifstream infile;
	infile.open(filename.c_str());
	if (infile.fail()) {
		std::cout << filename << " does not exist. Terminating program.\n";
		return 0;
	}

	std::string P2;
	int x_size;
	int y_size;
	int max_grey;

	infile >> P2;
	infile >> x_size;
	infile >> y_size;
	infile >> max_grey;

	std::vector<std::vector<int> > img_grid;
	img_grid.resize(x_size, std::vector<int>(y_size, 0));

	for (int y = 0; y < y_size; y++) {		// fill image grid from file
		for (int x = 0; x < x_size; x++) {
			infile >> img_grid[x][y];
		}
	}

	infile.close(); // close infile

	// remove vertical seams
	for (int i = 0; i < x_seams; i++) {
		remove_seam(x_size, y_size, img_grid);
	}

	// transpose image
	std::vector<std::vector<int> > new_img;
	img_grid.resize(x_size, std::vector<int>(y_size, 0));
	for (int y = 0; y < y_size; y++) {
		for (int x = 0; x < x_size; x++) {
			new_img[x][y] = img_grid[x][y];
		}
	}
	int new_x_size = y_size;
	int new_y_size = x_size;

	// remove horizontal seams
	for (int i = 0; i < y_seams; i++) {
		remove_seam(new_x_size, new_y_size, new_img);
	}

	// transpose image
	for (int y = 0; y < new_y_size; y++) {
		for (int x = 0; x < new_x_size; x++) {
			img_grid[x][y] = new_img[x][y];
		}
	}

	// write to new file
	std::string new_filename = filename.substr(0, filename.size() - 4) + "_processed.pgm";

	std::ofstream outfile;
	outfile.open(new_filename.c_str());

	outfile << "P2\n";
	outfile << x_size << " " << y_size << "\n";
	outfile << max_grey << "\n";
	for(int y = 0; y < y_size; y++) {
		for(int x = 0; x < x_size - 1; x++) {
			outfile << img_grid[x][y];
			if(x < x_size-1) {
				outfile << " ";
			}
		}
		outfile << "\n";
	}
	return 0;
}

// find smallest of 2 neighbors
int find_smallest(int a, int b) {
	if(a <= b) {
		return a;
	}
	else {
		return b;
	}
}

// find smallest of 3 neighbors
int find_smallest(int a, int b, int c) {
	if(a <= b && a <= c) {
		return a;
	}
	else if(b < a && b <= c) {
		return b;
	}
	else {
		return c;
	}
}

//trace the seam
void trace_seam(int x, int y, std::vector<int> &seam, std::vector<std::vector<int> > &grid) {
	seam[y] = x;
	if(grid[x][y] == grid[x-1][y-1]) {		// upper left neighbor
		trace_seam(x-1, y-1, seam, grid);
	}
	else if(grid[x][y] == grid[x][y-1]) {	// upper neighbor
		trace_seam(x, y-1, seam, grid);
	}
	else {									// upper right neighbor
		trace_seam(x+1, y-1, seam, grid);
	}
}

//remove the seam
void remove_seam(int& x_size, int& y_size, std::vector<std::vector<int> >& img_grid) {
	std::vector<std::vector<int> > energy_table;
	std::vector<std::vector<int> > cost_matrix;
	energy_table.resize(x_size, std::vector<int>(y_size, 0));
	cost_matrix.resize(x_size, std::vector<int>(y_size, 0));

	for (int y = 0; y < y_size; y++) {			// fill energy table from image grid
		for (int x = 0; x < x_size; x++) {
			if (x == 0) {						// no left
				if (y == 0) {					// no up
					energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x+1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y-1]);
				}
				else if (y == y_size - 1) {		// no down
					energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x+1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y-1]);
				}
				else {							// up and down
					energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x+1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y+1])
						+ std::abs(img_grid[x][y] - img_grid[x][y-1]);
				}
			}
			else if (x == x_size - 1) {			// no right
				if (y == 0) {					// no up
					energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x-1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y-1]);
				}
				else if (y == y_size - 1) {		// no down
					energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x-1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y-1]);
				}
				else {							// up and down
						energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x-1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y+1])
						+ std::abs(img_grid[x][y] - img_grid[x][y-1]);
					}
			}
			else {								// all else
				if (y == 0) {					// no up
					energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x-1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y+1])
						+ std::abs(img_grid[x][y] - img_grid[x+1][y]);
				}
				else if (y == y_size - 1) {		// no down
					energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x-1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y-1])
						+ std::abs(img_grid[x][y] - img_grid[x+1][y]);
				}
				else {							// all 4
					energy_table[x][y] = std::abs(img_grid[x][y] - img_grid[x+1][y])
						+ std::abs(img_grid[x][y] - img_grid[x][y+1])
						+ std::abs(img_grid[x][y] - img_grid[x][y-1])
						+ std::abs(img_grid[x][y] - img_grid[x][y+1]);
				}
			}
		}
	}

	// fill first line of cost matrix from energy table
	for(int x = 0; x < x_size; x++) {
		cost_matrix[x][0] = energy_table[x][0];
	}

	// fill rest of cost matrix
	for(int y = 1; y < y_size; y++) {
		for(int x = 0; x < x_size; x++) {
			if(x == 0) {					// no left
				cost_matrix[x][y] = energy_table[x][y] + find_smallest(cost_matrix[x][y-1], cost_matrix[x+1][y-1]);
			}
			else if(x == x_size - 1) {		// no right
				cost_matrix[x][y] = energy_table[x][y] + find_smallest(cost_matrix[x-1][y-1], cost_matrix[x][y-1]);
			}
			else {
				cost_matrix[x][y] = energy_table[x][y] + find_smallest(cost_matrix[x-1][y-1], cost_matrix[x][y-1],
					cost_matrix[x+1][y-1]);
			}
		}
	}

	// find smallest value in bottom row
	int starting_position = 0;
	int starting_value = energy_table[0][y_size - 1];

	for(int x = 0; x < x_size; x++) {
		if(cost_matrix[x][y_size - 1] < starting_value) {
			starting_position = x;
			starting_value = cost_matrix[x][y_size - 1];
		}
	}

	// trace the seam up from the bottom, record path
	std::vector<int> vertical_seam;
	vertical_seam.resize(y_size);
	trace_seam(starting_position, y_size, vertical_seam, img_grid);

	// remove seam and resize
	for(int y = 0; y < y_size; y++) {
		for(int x = vertical_seam[y]; x < x_size; x++) {
			img_grid[x][y] = img_grid[x+1][y];
		}
		img_grid[x_size - 1][y] = -1;
	}

	x_size = x_size - 1;
	y_size = y_size - 1;
}