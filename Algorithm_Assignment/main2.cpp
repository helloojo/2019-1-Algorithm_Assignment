#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cstring>
#include <list>
using namespace std;
char name[1000001][21];
bool flooding[1000001];
int tree[1000001];
int t;
vector<pair<int, int>> adj[1000001];
list<pair<int, int>> treeset;	//경로 저장위한 List 생성
pair<int, int> dijkstra(int src, int dest, char query) {
	if (flooding[src] || flooding[dest]) {
		return { -1,-1 };
	}
	priority_queue<pair<int, pair<int, int>>> pq;
	pq.push({ 0,{-src,-1} });
	int treesize = 0;
	while (!pq.empty()) {
		int dis = -pq.top().first;
		int here = -pq.top().second.first;
		int prev = pq.top().second.second;
		pq.pop();
		if (tree[here] == t) {
			continue;
		}
		if (query == 'B') {
			treeset.push_back({ prev,here });
		}
		tree[here] = t;
		treesize++;
		if (here == dest) {
			return { (dis > 1000000 ? -1 : dis),treesize };
		}
		int size = adj[here].size();
		for (int i = 0; i < size; i++) {
			int next = adj[here][i].second;
			int weight = adj[here][i].first + dis;
			if (tree[next] != t) {
				pq.push({ -weight,{-next,here} });
			}
		}
	}
	return { -1,-1 };
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);
	int n, m, q;
	cin >> n >> m >> q;
	int a, b, c;
	char s[21];
	for (int i = 0; i < n; i++) {
		cin >> a >> s >> c;
		strcpy(name[a], s);
		flooding[a] = c;
	}
	for (int i = 0; i < m; i++) {
		cin >> a >> b >> c;
		if (flooding[a] || flooding[b]) {
			continue;
		}
		adj[a].push_back({ c,b });
		adj[b].push_back({ c,a });
	}
	char d;
	pair<int, int> ret;
	vector<int> route;
	for (int i = 0; i < q; i++) {
		cin >> d >> a >> b;
		t++;
		ret = dijkstra(a, b, d);
		if (ret.first == -1) {
			cout << "None\n";
			continue;
		}
		switch (d) {
		case 'A':
			cout << ret.second << ' ' << ret.first << ' ' << name[a] << ' ' << name[b] << '\n';
			break;
		case 'B':
			cout << ret.second << ' ';
			int here = b;
			for (int j = 0; j < ret.second; ++j) {
				auto& p = treeset.back();
				if (p.second == here) {
					treeset.push_front(p);
					here = p.first;
				}
				treeset.pop_back();
			}
			//TreeSet에 저장된 경로 출력, O(N)
			while (!treeset.empty()) {
				auto& p = treeset.front();
				cout << p.second << ' ';
				treeset.pop_front();
			}
			cout << '\n';
			break;
		}
	}
	return 0;
}