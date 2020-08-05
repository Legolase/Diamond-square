#include <iostream>
#include <vector>
#include <random>
#include <conio.h>
#include <windows.h>
#include <algorithm>

using namespace std;

int rand(int left, int right) {
	mt19937 gen{ random_device()() };
	uniform_int_distribution<int> dist(left, right);
	return dist(gen);
}

void SetColour(int text, int bg) {
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((bg << 4) | text));
}

void setcur(int x, int y) // вывод без мерцания
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void show_error(const string& mess) {
	cout << mess;
	system("pause >nul");
	exit(0);
}

const char block = 219, empt = ' ', ccur = '+';
//const int deep[] = { 4, 12, 6, 14, 10, 2, 11, 3, 9, 1, 13, 5 };
const int deep[] = { 4, 12, 6, 14, 10, 2, 11, 3, 1, 5 };
//const int deep[] = { 4, 12, 6, 14, 10, 2, 11, 9, 1, 13, 5 };
const int deep_size = sizeof(deep) / sizeof(int);
int roughness = 20; //%

const int n = 11;
const int iarr[n][3] = { {0,0,1}, {0,0,-1}, {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {-1, 0, -1}, {-1, 0 , 1}, {1, 0 , 1}, {1, 0, -1} };
const char carr[n] = { 'w', 's', 'd', 'a', char(32), 'm', 'z', 'q', 'e', 'c'};

struct mpoint {
	int x = 0, y = 0;
	
	void set(const int x1, const int y1) {
		x = x1;
		y = y1;
	}
	bool operator<=(const mpoint& a) const {
		if ((this->x <= a.x) && (this->y <= a.y))
			return true;
		return false;
	}
};

struct square {
private:
	mpoint corners[2] = { {0, 0}, {0, 0} };
public:
	square(const mpoint a, const mpoint b) {
		corners[0] = a;
		corners[1] = b;
	}
	void set(const int a, const int b, const int c, const int d) {
		if ((a <= c) && (b <= d)) {
			corners[0].set(a, b);
			corners[1].set(c, d);
		}
		else 
			show_error("Err->square::set wasn't right!!!\n");
		return;
	}
	mpoint get_corner(const int index) const {
		if ((index > -1) && (index < 2))
			return corners[index];
		else
			show_error("Err -> indicated corner isn't");
	}
};

struct my_cursor {
	int x = 0, y = 0, z = 0;

	my_cursor operator+(const my_cursor& a) const{
		my_cursor b{this->x + a.x, this->y + a.y, this->z + a.z };
		return b;
	}
	my_cursor operator-(const my_cursor& a) const {
		my_cursor b{ this->x - a.x, this->y - a.y, this->z - a.z };
		return b;
	}
	void operator=(const my_cursor& a) { this->x = a.x; this->y = a.y; this->z = a.z; }
	bool operator==(const my_cursor& a) const { return ((this->x == a.x) && (this->y == a.y) && (this->z == a.z)) ? true : false; }
};

struct sps {
	char c = ' ';
	int colour = 7;
};

int fib(unsigned int index) {
	int first = 0, second = 1, third = 2;

	for (int i = 0; i < index; ++i) {
		first = second;
		second = third;
		third = 0;

		third = first + second;
	}
	return third;
}

void set_centers(vector<vector<int>>& map, const vector<square>& diap) {
	for (int i = 0; i < diap.size(); ++i) {
		const mpoint ld = diap[i].get_corner(0), ru = diap[i].get_corner(1);
		const mpoint lu{ ld.x, ru.y }, rd{ ru.x, ld.y };
		if ((map[ld.x][ld.y] != -1) && (map[lu.x][lu.y] != -1) && (map[rd.x][rd.y] != -1) && (map[ru.x][ru.y] != -1)) {
			const mpoint center{ ceil(((rd.x - ld.x) + 1) / 2.0) - 1 + ld.x , ceil(((lu.y - ld.y) + 1) / 2.0) - 1 + ld.y };
			const int distance = lu.y - ld.y;
			map[center.x][center.y] = round(((map[ld.x][ld.y] + map[lu.x][lu.y] + map[ru.x][ru.y] + map[rd.x][rd.y]) / 4.0) + ((distance * sqrt(2)) * (rand(roughness * (-1), roughness) / 100.0)));

			if (map[center.x][center.y] < 0)
				map[center.x][center.y] = 0;
		}
		else
			show_error("Error with heights (-1)");
	}
}

bool in_square(const mpoint& a, const vector<vector<int>>& sqe) {
	return ((a.x > -1) && (a.y > -1) && (a.x < (int)sqe.size()) && (a.y < sqe[0].size())) ? true : false;
}

vector<square> mcut(vector<vector<int>>& mmap, const square& old_square) {
	const mpoint ld = old_square.get_corner(0), ru = old_square.get_corner(1);
	const mpoint lu{ ld.x, ru.y }, rd{ ru.x, ld.y };

	if ((mmap[ld.x][ld.y] != -1) && (mmap[lu.x][lu.y] != -1) && (mmap[rd.x][rd.y] != -1) && (mmap[ru.x][ru.y] != -1)) {
		const mpoint center{ ceil(((rd.x - ld.x) + 1) / 2.0) - 1 + ld.x , ceil(((lu.y - ld.y) + 1) / 2.0) - 1 + ld.y };
		const mpoint l{ ld.x, center.y }, u{ center.x, lu.y }, r{ rd.x, center.y }, d{ center.x, ld.y };
		const int distance = lu.y - ld.y;
		const int half_distance = distance/2;
		const mpoint ol{ ld.x - half_distance, l.y }, ou{ u.x, u.y + half_distance }, our{ r.x + half_distance, r.y }, od{ d.x, d.y - half_distance };

		if (in_square(ol, mmap))
			mmap[l.x][l.y] = round(((mmap[ol.x][ol.y] + mmap[lu.x][lu.y] + mmap[center.x][center.y] + mmap[ld.x][ld.y]) / 4.0) + (distance * (rand(roughness * (-1), roughness) / 100.0)));
		else
			mmap[l.x][l.y] = round(((mmap[lu.x][lu.y] + mmap[center.x][center.y] + mmap[ld.x][ld.y]) / 3.0) + (distance * (rand(roughness * (-1), roughness) / 100.0)));

		if (in_square(ou, mmap))
			mmap[u.x][u.y] = round(((mmap[ou.x][ou.y] + mmap[ru.x][ru.y] + mmap[center.x][center.y] + mmap[lu.x][lu.y]) / 4.0) + (distance * (rand(roughness * (-1), roughness) / 100.0)));
		else
			mmap[u.x][u.y] = round(((mmap[ru.x][ru.y] + mmap[center.x][center.y] + mmap[lu.x][lu.y]) / 3.0) + (distance * (rand(roughness * (-1), roughness) / 100.0)));

		if (in_square(our, mmap))
			mmap[r.x][r.y] = round(((mmap[our.x][our.y] + mmap[rd.x][rd.y] + mmap[center.x][center.y] + mmap[ru.x][ru.y]) / 4.0) + (distance * (rand(roughness * (-1), roughness) / 100.0)));
		else
			mmap[r.x][r.y] = round(((mmap[rd.x][rd.y] + mmap[center.x][center.y] + mmap[ru.x][ru.y]) / 3.0) + (distance * (rand(roughness * (-1), roughness) / 100.0)));

		if (in_square(od, mmap))
			mmap[d.x][d.y] = round(((mmap[od.x][od.y] + mmap[ld.x][ld.y] + mmap[center.x][center.y] + mmap[rd.x][rd.y]) / 4.0) + (distance * (rand(roughness * (-1), roughness) / 100.0)));
		else
			mmap[d.x][d.y] = round(((mmap[ld.x][ld.y] + mmap[center.x][center.y] + mmap[rd.x][rd.y]) / 3.0) + (distance * (rand(roughness * (-1), roughness) / 100.0)));

		/*(mmap[l.x][l.y] == -1) ? mmap[l.x][l.y] = round(((mmap[ld.x][ld.y] + mmap[lu.x][lu.y]) / 2.0) + (distance * (rand(roughness * (-1), roughness) / 100.0))) : 1 ;
		(mmap[u.x][u.y] == -1) ? mmap[u.x][u.y] = round(((mmap[lu.x][lu.y] + mmap[ru.x][ru.y]) / 2.0) + (distance * (rand(roughness * (-1), roughness) / 100.0))) : 1 ;
		(mmap[r.x][r.y] == -1) ? mmap[r.x][r.y] = round(((mmap[rd.x][rd.y] + mmap[ru.x][ru.y]) / 2.0) + (distance * (rand(roughness * (-1), roughness) / 100.0))) : 1 ;
		(mmap[d.x][d.y] == -1) ? mmap[d.x][d.y] = round(((mmap[ld.x][ld.y] + mmap[rd.x][rd.y]) / 2.0) + (distance * (rand(roughness * (-1), roughness) / 100.0))) : 1 ;*/

		if (mmap[l.x][l.y] < 0)
			mmap[l.x][l.y] = 0;
		if (mmap[u.x][u.y] < 0)
			mmap[u.x][u.y] = 0;
		if (mmap[r.x][r.y] < 0)
			mmap[r.x][r.y] = 0;
		if (mmap[d.x][d.y] < 0)
			mmap[d.x][d.y] = 0;

		return { {ld, center}, {d, r}, {l, u}, {center, ru} };
	}
	else
		show_error("Error with heights (-1)");

}

void get_squares(vector<vector<int>>& mmap, vector<square>& old_squares) {
	vector<square> new_squares;

	set_centers(mmap, old_squares);

	for (int i = 0; i < old_squares.size(); ++i) {
		vector<square> new_part = mcut(mmap, old_squares[i]);
		
		for (int j = 0; j < new_part.size(); ++j)
			new_squares.push_back(new_part[j]);
	}
	old_squares = new_squares;
	return;
}

void make_heights(vector<vector<int>>& map, const int& size, const vector<int>& start_heights) {
	const int length = pow(2, size) + 1;
	map.resize(length, vector<int>(length));

	for (int i = 0; i < length; ++i)
		for (int j = 0; j < length; ++j)
			map[i][j] = -1;

	map[0][0] = rand(start_heights[0], start_heights[1]);
	map[0][length-1] = rand(start_heights[0], start_heights[1]);
	map[length-1][0] = rand(start_heights[0], start_heights[1]);
	map[length - 1][length - 1] = rand(start_heights[0], start_heights[1]);
	
	vector<square> squares = { {{0, 0}, {length - 1, length - 1}} };

	while (true) {
		{
			mpoint a = squares[0].get_corner(0), b = squares[0].get_corner(1);

			if (b.x - a.x == 1)
				break;
		}

		get_squares(map, squares);
	}

}

bool in_box(my_cursor cursorm, const vector<vector<vector<char>>>& room) {
	return ((cursorm.x > -1) && (cursorm.y > -1) && (cursorm.z > -1) && (cursorm.x < (int)room.size()) && (cursorm.y < (int)room[0].size()) && (cursorm.z < (int)room[0][0].size())) ? true : false;
}

void show(const vector<vector<vector<char>>>& place, my_cursor curs) {
	vector<vector<sps>> top((int)place.size(), vector<sps>(place[0][0].size())), forward((int)place.size(), vector<sps>((int)place[0].size())), bok(place[0][0].size(), vector<sps>((int)place[0].size()));

	for (int x = 0; x < (int)place.size(); ++x)
		for (int z = 0; z < place[0][0].size(); ++z)
			for (int i = 0; i < deep_size; ++i) {
				if ((in_box({x, curs.y - i, z}, place)) && (place[x][curs.y - i][z] == block)) {
					top[x][z].c = block;
					top[x][z].colour = deep[i];
					break;
				}
			}

	for (int x = 0; x < (int)place.size(); ++x)
		for (int y = 0; y < (int)place[0].size(); ++y)
			for (int i = 0; i < deep_size; ++i) {
				if ((in_box({ x, y, curs.z + i }, place)) && (place[x][y][curs.z + i] == block)) {
					forward[x][y].c = block;
					forward[x][y].colour = deep[i];
					break;
				}
			}

	for (int z = 0; z < place[0][0].size(); ++z)
		for (int y = 0; y < (int)place[0].size(); ++y)
			for (int i = 0; i < deep_size; ++i) {
				if ((in_box({ curs.x + i, y, z }, place)) && (place[curs.x + i][y][z] == block)) {
					bok[z][y].c = block;
					bok[z][y].colour = deep[i];
					break;
				}
			}

	for (int j = max(top[0].size(), max(forward[0].size(), bok[0].size())) - 1; j > -1; --j) {
		for (int i = 0; i < (int)top.size(); ++i) {
			if (j < top[0].size()) {
				if ((j == curs.z) && (i == curs.x)) {
					SetColour(7, 0);
					cout << '+';
				}
				else {
					SetColour(top[i][j].colour, 0);
					cout << top[i][j].c;
				}
			}
			else {
				SetColour(7, 0);
				cout << '#';
			}
		}
		SetColour(7, 0);
		cout << "||";

		for (int i = 0; i < (int)forward.size(); ++i) {
			if (j < forward[0].size()) {
				if ((j == curs.y) && (i == curs.x)) {
					SetColour(7, 0);
					cout << '+';
				}
				else {
					SetColour(forward[i][j].colour, 0);
					cout << forward[i][j].c;
				}
			}
			else {
				SetColour(7, 0);
				cout << '#';
			}
		}
		SetColour(7, 0);
		cout << "||";

		for (int i = (int)bok.size() - 1; i > -1 ; --i) {
			if (j < bok[0].size()) {
				if ((j == curs.y) && (i == curs.z)) {
					SetColour(7, 0);
					cout << '+';
				}
				else {
					SetColour(bok[i][j].colour, 0);
					cout << bok[i][j].c;
				}
			}
			else {
				SetColour(7, 0);
				cout << '#';
			}
		}
		SetColour(7, 0);
		cout << "||\n";
	}

	const int full_length = (int)top.size() + (int)forward.size() + (int)bok.size();
	for (int i = 0; i < full_length + 6; ++i)
		cout << "=";
	cout << '\n';

	for (int i = 0; i < deep_size; ++i) {
		SetColour(deep[i], 0);
		cout << block;
	}

	/*for (int j = top[0].size() - 1; j > -1; --j) {
		for (int i = 0; i < (int)top.size(); ++i) {
			if ((j == curs.z) && (i == curs.x)) {
				SetColour(7, 0);
				cout << '+';
			}
			else {
				SetColour(top[i][j].colour, 0);
				cout << top[i][j].c;
			}
		}
		SetColour(7, 0);
		cout << "||\n";
	}

	SetColour(7, 0);
	for (int i = 0; i < (int)top.size() + 2; ++i)
		cout << '=';
	cout << '\n';

	for (int j = forward[0].size() - 1; j > -1; --j) {
		for (int i = 0; i < (int)forward.size(); ++i) {
			if ((j == curs.y) && (i == curs.x)) {
				SetColour(7, 0);
				cout << '+';
			}
			else {
				SetColour(forward[i][j].colour, 0);
				cout << forward[i][j].c;
			}
		}
		SetColour(7, 0);
		cout << "||\n";
	}

	SetColour(7, 0);
	for (int i = 0; i < (int)forward.size() + 2; ++i)
		cout << '=';
	cout << '\n';

	for (int j = bok[0].size() - 1; j > -1; --j) {
		for (int i = 0; i < (int)bok.size(); ++i) {
			if ((j == curs.y) && (i == curs.z)) {
				SetColour(7, 0);
				cout << '+';
			}
			else {
				SetColour(bok[i][j].colour, 0);
				cout << bok[i][j].c;
			}
		}
		SetColour(7, 0);
		cout << "||\n";
	}

	SetColour(7, 0);
	for (int i = 0; i < (int)bok.size() + 2; ++i)
		cout << '=';
	cout << '\n';*/
}

int main() {
	vector<int> sh = { 10, 15 };
	bool bexit = false;
	my_cursor cur;
	while (!bexit) {
		vector<vector<int>> map1;
		make_heights(map1, 6, sh);
		const int size = map1[0].size();
		vector<vector<vector<char>>> box(size, vector<vector<char>>(size, vector<char>(size)));

		for (int x = 0; x < size; ++x) //filling space with blocks
			for (int y = 0; y < size; ++y)
				for (int z = 0; z < size; ++z) {
					if (y <= map1[x][z])
						box[x][y][z] = block;
					else
						box[x][y][z] = empt;
				}
		//cur.y = map1[cur.x][cur.z] + 1;
		setcur(0, 0);
		show(box, cur);
		while (true) {

			char inputt = _getch();

			if (inputt == '0') {
				bexit = true;
				break;
			}
			else if (inputt == '1')
				break;
			else if (inputt == '2') {
				cin >> roughness;
				system("cls");
			}
			else if (inputt == '3') {
				cin >> sh[0] >> sh[1];
				system("cls");
			}
			else {
				int i = 0;
				bool found = false;
				for (i = 0; i < n; ++i) {
					if (inputt == carr[i]) {
						found = true;
						break;
					}
				}
				if (found) {
					my_cursor step{ iarr[i][0], iarr[i][1], iarr[i][2] };
					my_cursor destination = cur + step;
					/*if (in_box(destination, box)) {
						if (box[destination.x][destination.y][destination.z] == 32) {
							while (destination.y > 0) {
								if (box[destination.x][destination.y - 1][destination.z] == -37) {
									cur = destination;
									break;
								}
								else
									--destination.y;
							}
							if (destination.y == 0)
								cur = destination;
						}
						else if ((box[destination.x][destination.y][destination.z] == -37) && (in_box({ destination.x, destination.y + 1, destination.z }, box)) && (box[destination.x][destination.y + 1][destination.z] == 32)) {
							++destination.y;
							cur = destination;
						}
					}*/
					if (in_box(destination, box)) 
						cur = destination;
				}
			}
			setcur(0, 0);
			show(box, cur);
		}
	}
}