#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <bitset>
#include <deque>
#include <queue>
#include <tuple>
#include <cmath>
#include <cctype>
#include <stack>
#include <cassert>
#include <iomanip>
using namespace std;
using LL = long long;
LL MXVAL = 0x7fffffffffffffff;
LL N, K, D;

class Point {
public:
	LL x, y;
	int vld;

	// comparison is done first on y coordinate and then on x coordinate
	bool operator < (Point b) {
		if (y != b.y)
			return y < b.y;
		return x < b.x;
	}
	bool operator == (Point b) {
		if (y != b.y)
			return 0;
		return x == b.x;
	}
};

Point points[1000005];

// Point having the least y coordinate, used for sorting other points
// according to polar angle about this point
Point pivot;

// returns -1 if a -> b -> c forms a counter-clockwise turn,
// +1 for a clockwise turn, 0 if they are collinear
int ccw(Point a, Point b, Point c) {
	LL area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
	if (area > 0)
		return -1;
	else if (area < 0)
		return 1;
	return 0;
}

// returns square of Euclidean distance between two points
LL sqrDist(Point a, Point b) {
	LL dx = a.x - b.x, dy = a.y - b.y;
	return dx * dx + dy * dy;
}

// used for sorting points according to polar order w.r.t the pivot
bool POLAR_ORDER(Point a, Point b) {
	int order = ccw(pivot, a, b);
	if (order == 0)
		return sqrDist(pivot, a) < sqrDist(pivot, b);
	return (order == -1);
}

stack<Point> grahamScan(int N) {
	stack<Point> hull;
	if (N < 3)
		return hull;
	// sort the remaining point according to polar order about the pivot
	pivot = points[0];
	sort(points + 1, points + N, POLAR_ORDER);

	int ccv = ccw(points[0], points[1], points[2]);
	if (ccv == 0) {
		points[0].vld = 0;
		points[1].vld = 0;
		points[2].vld = 0;
	}
	hull.push(points[0]);
	hull.push(points[1]);
	hull.push(points[2]);

	for (int i = 3; i < N; i++) {
		Point top = hull.top();
		hull.pop();
		int cv = ccw(hull.top(), top, points[i]);
		while (cv != -1) {
			if (cv == 0) {
				hull.top().vld = 0;
				top.vld = 0;
				points[i].vld = 0;
			}
			top = hull.top();
			hull.pop();
			cv = ccw(hull.top(), top, points[i]);
		}
		hull.push(top);
		hull.push(points[i]);
	}
	return hull;
}

LL search(vector<Point> &pl, int l, int r, map<pair<int, int>, LL> &v, LL D) {
	if (l >= r) return 0;
	auto itr = v.find(pair<int, int>(l, r));
	if (itr != v.end()) return itr->second;
	LL val = MXVAL, vl = sqrDist(pl[l], pl[r]);
	if (vl <= D)
		val = vl;
	for (int i = l + 1; i < r; ++i) {
		LL tp1 = search(pl, l, i, v, D);
		if (tp1 < 0 || tp1 > val) continue;
		LL tp2 = search(pl, i, r, v, D);
		if (tp2 < 0) continue;
		LL tp = tp1 + tp2;
		if (tp < val)
			val = tp;
	}
	val = max(K, val);
	if (val == MXVAL)
		return -1;
	v.insert(pair<pair<int, int>, LL>(pair<int, int>(l, r), val));
	return val;
}

LL chk(vector<Point> &pl, LL K, LL D) {
	LL val = 0;
	Point prv = pl[0];
	for (auto p : pl) {
		LL dd = sqrDist(p, prv);
		if (dd > D) return -1;
		val += max(K, dd);
		prv = p;
	}

	map<pair<int, int>, LL> v;
	return search(pl, 0, pl.size() - 1, v, D);
}

int main() {
	//freopen("lemonade_life_validation_inputs.txt", "r", stdin);
	//freopen("lemonade_life_validation_input.txt", "r", stdin);
	freopen("lemonade_life_input.txt", "r", stdin);
	//freopen("d.txt", "w", stdout);
	int T; cin >> T;
	for (int t = 1; t <= T; t++) {
		cin >> N >> K >> D;
		D = D * D;
		
		LL mxval = MXVAL;
		LL mxy = -1;
		Point p_start, p_end;
		for (int i = 0; i < N; i++) {
			int x, y; cin >> y >> x;
			points[i].x = x;
			points[i].y = y;
			points[i].vld = 1;
			if (y > mxy) { mxy = y; p_end = points[i]; }
			//("(%d, %d)\n", x, y);
		}

		p_start = points[0];
		stack<Point> hull = grahamScan(N);
		vector<Point> pl = {p_start, p_end};
		while (!hull.empty()) {
			Point p = hull.top();
			hull.pop();
			// printf("(%d, %d, %d)", p.x, p.y, p.vld);
			if (p == p_start || p == p_end) continue;
			if (p.vld) pl.push_back(p);
		}
		// printf("\n"); for (auto &pp : pl) printf("(%d, %d, %d)", pp.x, pp.y, pp.vld); printf("\n");
		sort(pl.begin(), pl.end());
		// printf("\n");for (auto &pp : pl) printf("(%d, %d, %d)", pp.x, pp.y, pp.vld); printf("\n");

		printf("%d,%d\n",T,pl.size());

		cout << "Case #" << t << ": " << chk(pl, K, D) << endl;
	}
	//system("pause");
}