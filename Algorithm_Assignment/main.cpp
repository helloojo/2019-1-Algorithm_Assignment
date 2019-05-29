#include <stdio.h>

void m_strcpy(char* dest, char* src) {
	//문자열 복사 함수
	register int i = 0;
	while (src[i]) {
		dest[i] = src[i];
		++i;
	}
	dest[i] = '\0';
}

template <class a, class b>
struct Pair {
	a first;
	b second;
};

#define LIST_POOL_SIZE 800001

template <class T>
class List {
	/*
	STL list 비슷하게 구현
	Double Linked List
	모든 연산 시간복잡도 O(1)
	*/
private:
	//Linked List Node
	struct Node {
		Node* prev;
		Node* next;
		T data;
	};
	//실행 속도를 위한 NodePool
	static Node Pool[LIST_POOL_SIZE];
	//NodePool count 변수
	static int cnt;
	//List의 마지막 Node 의미
	Node endNode;
public:
	//List 탐색을 위한 양방향 Iterator 구현
	class Iterator {
	private:
		Node* point;	//List 위치 저장
		inline Node* getpoint() const {
			return point;
		}
		friend class List<T>;	//List class와 friend class 선언
	public:
		Iterator(Node* p = nullptr) : point(p) {}
		//iterator 연산자 오버로딩
		inline Iterator& operator++() {
			if (point->next) {
				point = point->next;
			}
			return (*this);
		}
		inline Iterator operator++(int) {
			Iterator temp(point);
			if (point->next) {
				point = point->next;
			}
			return temp;
		}
		inline Iterator& operator--() {
			if (point->prev) {
				point = point->prev;
			}
			return (*this);
		}
		inline Iterator& operator--(int) {
			Iterator temp(point);
			if (point->prev) {
				point = point->prev;
			}
			return temp;
		}
		inline T& operator*() const {
			return (*point).data;
		}
		inline T& operator->() const {
			return point->data;
		}
		inline Iterator& operator=(Node* p) {
			point = p;
			return (*this);
		}
		inline bool operator==(Iterator& it) const {
			return it.point == point;
		}
		inline bool operator!=(Iterator& it) const {
			return !((*this) == it);
		}
	};
	typedef Iterator iterator;
private:
	iterator head;
	iterator tail;
	int list_size;	//list size 저장 변수
	//NodePool에서 Node가져와 초기화 후 반환
	Node* make_node(const T& data = T()) {
		Node* new_node = &Pool[cnt++];
		new_node->data = data;
		new_node->prev = new_node->next = nullptr;
		return new_node;
	}
public:
	/*
	List 초기화 생성자
	head,tail에 List의 끝을 의미하는 endNode 연결
	*/
	List() : endNode({ nullptr,nullptr,T() }), head(&endNode), tail(&endNode), list_size(0) {}
	//Head에 data Node 추가하는 함수
	void push_front(const T& data) {
		Node* new_node = make_node(data);
		new_node->next = head.getpoint();
		head.getpoint()->prev = new_node;
		head = new_node;	//head에 추가된 노드 연결
		++list_size;
	}
	//tail에 data Node 추가하는 함수
	void push_back(const T& data) {
		Node* new_node = make_node(data);
		if (!list_size) {
			head = new_node;
		}
		Node* prev = tail.getpoint()->prev;
		if (prev) {
			prev->next = new_node;
		}
		new_node->prev = prev;
		tail.getpoint()->prev = new_node;	//tail 전에 Node추가
		new_node->next = tail.getpoint();
		++list_size;
	}
	//tail의 data Node 하나 제거하는 함수
	void pop_back() {
		if (!list_size) {
			throw "No Element";
		}
		Node* temp = tail.getpoint()->prev;
		Node* prev = temp->prev;
		if (prev) {
			prev->next = tail.getpoint();
		}
		tail.getpoint()->prev = prev;
		--list_size;
	}
	//head의 data Node 하나 제거하는 함수
	void pop_front() {
		if (!list_size) {
			throw "No Element";
		}
		Node* temp = head.getpoint();
		Node* next = temp->next;
		if (next) {
			next->prev = nullptr;
		}
		head = next;
		--list_size;
	}
	//head의 원소 반환 함수, const와 non-const 두 버전 존재
	const T& front() const {
		if (!list_size) {
			throw "No Element";
		}
		return *head;
	}
	T& front() {
		return const_cast<T&>(static_cast<const List&>(*this).front());
	}
	//tail의 원소 반환 함수, const와 non-const 두 버전 존재
	const T& back() const {
		if (!list_size) {
			throw "No Element";
		}
		iterator t = tail.getpoint()->prev;
		return *t;
	}
	T& back() {
		return const_cast<T&>(static_cast<const List&>(*this).back());
	}
	//head iterator 반환 함수
	iterator& begin() {
		return head;
	}
	//tail iterator 반환 함수
	iterator& end() {
		return tail;
	}
	//list_size 반환 함수
	int size() const {
		return list_size;
	}
	//List empty여부 반환 함수
	bool empty() const {
		return !list_size;
	}
};

//List의 Static 변수 초기화
template <class T>
int List<T>::cnt = 0;
template <class T>
typename List<T>::Node List<T>::Pool[LIST_POOL_SIZE];

//Edge, FringeSet을 위한 Vertex Class 선언
class Vertex;

//Heap을 위한 FringeSet 구조체 선언
struct FringeSet {
	int weight;	//d(src,i)+W(ij)저장
	int prev;	//i Vertex의 key 저장
	int herenum;//j Vertex의 key 저장
	int order;	//Priority Queue에 insert된 순서 저장
	Vertex* here;//j Vertex를 가리키는 포인터
};

/*
FringeSet 비교 연산자 오버로딩
weight를 비교하고
weight가 같으면 j Vertex의 key를 비교하고,
j Vertex의 key가 같으면 Priority Queue에 삽입된 순서를 비교한다.
*/
bool operator<(const FringeSet& a, const FringeSet& b) {
	if (a.weight == b.weight) {
		if (a.herenum == b.herenum) {
			return a.order < b.order;
		}
		return a.herenum < b.herenum;
	}
	return a.weight < b.weight;
}
bool operator>(const FringeSet& a, const FringeSet& b) {
	if (a.weight == b.weight) {
		if (a.herenum == b.herenum) {
			return a.order > b.order;
		}
		return a.herenum > b.herenum;
	}
	return a.weight > b.weight;
}

class Priority_Queue {
	/*
	minheap을 구현
	weight 최소값이 Top에 위치
	*/
private:
	//최대 400000의 Edge가 존재하므로 Heap크기 400000으로 설정
	FringeSet heap[400001];
	int pos;	//Heap의 Data가 삽입될 위치
	/*
	UpHeap 연산 구현
	parent와 비교한 후,
	parent보다 작으면 parent에서 upheap 수행,
	크면 data 저장
	시간복잡도 O(logN)
	*/
	void up_heap(int here, const FringeSet& data) {
		if (here == 1) {
			heap[here] = data;
			return;
		}
		int parent = here / 2;
		if (heap[parent] < data) {
			heap[here] = data;
		} else {
			heap[here] = heap[parent];
			up_heap(parent, data);
		}
	}
	/*
	DownHeap 연산 구현
	left, right child중 min값 구한 후,
	data와 비교해 minchild가 더 크면 위치에 삽입,
	작으면 minchild 위치에서 downheap 수행
	시간복잡도 2logN -> O(logN)
	*/
	void down_heap(int here, const FringeSet& data) {
		if (heap[here].weight == 987654321) {
			heap[here] = data;
			return;
		}
		int left = here * 2;
		int right = here * 2 + 1;
		int minidx = (heap[left] > heap[right] ? right : left);
		FringeSet& minchild = heap[minidx];
		if (minchild > data || minchild.weight == 987654321) {
			heap[here] = data;
		} else {
			heap[here] = minchild;
			down_heap(minidx, data);
		}
	}
public:
	//Priority_Queue 초기화, O(M)
	Priority_Queue() : pos(1) {
		for (int i = 0; i < 400001; ++i) {
			heap[i] = { 987654321,0,0,0,nullptr };
		}
	}
	void clear() {
		for (int i = 1; i < pos; ++i) {
			heap[i] = { 987654321,0,0,0,nullptr };
		}
		pos = 1;
	}
	//Priority_Queue에 원소 삽입, O(logN)
	void push(const FringeSet& data) {
		heap[pos++] = data;
		up_heap(pos - 1, data);
	}
	//Priority_Queue top(min value) 반환
	FringeSet& top() {
		return heap[1];
	}
	//Priority_Queue Empty여부 반환
	bool empty() const {
		return pos == 1;
	}
	//Priority_Queue에 원소 삭제, O(logN)
	void pop() {
		pos--;
		FringeSet item = heap[pos];
		heap[pos] = { 987654321,0,0,0,nullptr };
		down_heap(1, item);
	}
};


class Edge {
	/*
	Edge Class
	시작 Vertex, 끝 Vertex, 거리를 저장
	*/
private:
	Vertex* src;
	Vertex* dest;
	int distance;
public:
	Edge() : src(nullptr), dest(nullptr), distance(0) {}
	//Edge 초기화 함수
	void init(Vertex* s, Vertex* d, int dis) {
		src = s;
		dest = d;
		distance = dis;
	}
	//Distance 반환 함수
	int getdistance() const {
		return distance;
	}
	//시작 Vertex 반환 함수
	Vertex* getsrc() const {
		return src;
	}
	//도착 Vertex 반환 함수
	Vertex* getdest() const {
		return dest;
	}
};
//EdgePool과, 사용중인 Pool개수를 파악하기 위한 count변수
static Edge E_Pool[400001];
static int e_cnt = 0;

class Vertex {
	/*
	Vertex Class
	인접한 정점을 저장하는 인접리스트,
	Key, Vertex이름, 침수여부, 상태(Tree, Fringe or Unseen) 저장
	*/
private:
	List<Edge*> adj_list;
	int key;	//지역번호
	char name[21];	//지역이름
	bool flooding;	//침수여부
	int state;
public:
	Vertex() : key(0), flooding(false), state(0) {}
	//Vertex 초기화 함수
	void init(int k, char* n, bool f) {
		m_strcpy(name, n);
		key = k;
		flooding = f;
	}
	//인접리스트에 Edge 추가
	void addEdge(Edge* new_edge) {
		adj_list.push_back(new_edge);
	}
	//Vertex의 이름 반환
	const char* getname() const {
		return name;
	}
	//Vertex의 Key 반환
	int getdata() const {
		return key;
	}
	//Vertex의 침수여부 반환
	bool isflooding() const {
		return flooding;
	}
	//Vertex가 Treeset인지 반환
	bool istree(int code) const {
		return state == code;
	}
	//Vertex를 Treeset으로 설정
	void settree(int code) {
		state = code;
	}
	//Vertex와 incident한 Edge(인접리스트)반환
	List<Edge*>& incident_edges() {
		return adj_list;
	}
};
//VertexPool과, 사용중인 Pool개수를 파악하기 위한 count변수
static Vertex V_Pool[100001];
static int v_cnt = 0;

class Graph {
	/*
	Graph Class
	vertex의 Key값으로 Vertex를 찾을 수 있는 포인터 배열,
	방향 그래프 여부 저장
	*/
private:
	Vertex* vertex[1000000];
	bool digraph;
	Vertex* make_vertex(int key, char* name, bool flooding) {
		//Vertex Pool에서 Vertex가져와 데이터 초기화 후 추가, O(1)
		Vertex* new_vertex = &V_Pool[v_cnt++];
		new_vertex->init(key, name, flooding);
		vertex[key] = new_vertex;
		return new_vertex;
	}
	void make_edge(Vertex* src, Vertex* dest, int distance) {
		//Edge Pool에서 Edge가져와 데이터 초기화 후 추가, O(1)
		Edge* new_edge = &E_Pool[e_cnt++];
		new_edge->init(src, dest, distance);
		//src의 인접리스트에 Edge 추가, O(1)
		src->addEdge(new_edge);
	}
public:
	//digraph 변수가 true일경우 유향그래프
	Graph(bool digraph = true) : digraph(digraph) {}
	//Vertex 추가함수, O(1)
	void insert_vertex(int key, char* name, bool flooding) {
		make_vertex(key, name, flooding);
	}
	//Edge 추가함수, O(1)
	void insert_edge(int s, int d, int distance) {
		//시작정점 탐색, O(1)
		Vertex* src = find_vertex(s);
		//도착정점 탐색, O(1)
		Vertex* dest = find_vertex(d);
		if (src->isflooding() || dest->isflooding()) {
			return;
		}
		//src->dest Edge 생성, O(1)
		make_edge(src, dest, distance);
		//무향그래프일경우 반대 Edge도 추가, O(1)
		if (!digraph) {
			make_edge(dest, src, distance);
		}
	}
	//Vertex 탐색함수, O(1)
	Vertex* find_vertex(int key) {
		return vertex[key];
	}
};

//fringe set 생성
Priority_Queue fringe;
Pair<int, int> dijkstra(Graph& g, List<Pair<int, int>>& tree, Vertex* src, Vertex* dest, int code, char query) {
	/*
	Dijkstra Algorithm 구현
	query에 따라 경로를 저장(B일때 저장)
	반환값 : Pair<int,int>
	first -> 최단 거리(10^6 초과 시 -1)
	second -> TreeSet 크기
	시작, 도착지점 중 침수지역 존재하거나, 경로 존재하지 않을 시
	{-1,-1} 반환
	*/
	if (src->isflooding() || dest->isflooding()) {
		//시작, 도착지점 중 침수 존재
		return { -1,-1 };
	}
	register int order = 0;	//FringeSet에 들어간 순서 결정
	register int tree_size = 0;	//TreeSet의 크기 저장
	fringe.push({ 0,-1,src->getdata(),++order,src });	//시작정점 Fringe Set에 추가

	//Fringe Set에 들어가는 Vertex 최대 M, FringeSet 연산 복잡도 O(logM)
	while (!fringe.empty()) {
		Vertex* s = fringe.top().here;	//d(src,i)+W(is)가 최소가 되는 Vertex S, O(1)
		int here = fringe.top().herenum;
		int prev = fringe.top().prev;	//i Vertex의 Key, O(1)
		int dis = fringe.top().weight;	//d(src,i)+W(is), O(1)
		fringe.pop();		//fringeSet에서 제거, O(logM), 총 M*logM
		if (s->istree(code)) {	//S가 TreeSet에 존재하면 계산하지 않음
			continue;
		}
		s->settree(code);	//S를 TreeSet에 추가
		if (query == 'B') {	//경로를 저장해야하는 Query일 경우 저장(이전Vertex,현재Vertex)
			tree.push_back({ prev,here });
		}
		++tree_size;	//treeset의 크기증가
		if (s == dest) {
			//S Vertex가 도착정점일경우
			//fringe set 초기화 후 도착정점까지의 거리와 Treeset 크기 반환
			fringe.clear();
			return { (dis > 1000000 ? -1 : dis), tree_size };
		}
		//auto -> List<Edge*>&
		auto& adj_list = s->incident_edges();
		auto e = adj_list.end();
		//incident Edge를 통해 S와 인접한 정점 모두 탐색, O(deg(s)), 총 sigma(s->V)(deg(s)) -> O(M)
		for (auto i = adj_list.begin(); i != e; ++i) {
			Vertex* next = i->getdest();	//다음 Vertex n
			int weight = dis + i->getdistance(); //d(src,s)+W(sn)
			//Vertex n이 TreeSet이 아닐경우 FringeSet에 추가, O(logM), 총 O(MlogM)
			if (!next->istree(code)) {
				fringe.push({ weight,s->getdata(),next->getdata(),++order,next });
			}
		}
	}
	//시간복잡도 O(MlogM)+O(MlogM) -> O(MlogM),(M<N^2) -> O(MlogN^2) -> O(2MlogN) -> O(MlogN)
	return { -1,-1 };
}

Graph g(false);	//그래프 생성
List<Pair<int, int>> tree;	//경로 저장위한 List 생성

int main() {
	int n, m, q;
	char name[21] = "";
	int in1, in2, in3;
	register int i, j;
	scanf("%d %d %d", &n, &m, &q);
	//Vertex 추가
	for (i = 0; i < n; ++i) {
		scanf("%d %s %d", &in1, name, &in3);
		g.insert_vertex(in1, name, in3);
	}
	//Edge 추가
	for (i = 0; i < m; ++i) {
		scanf("%d %d %d", &in1, &in2, &in3);
		g.insert_edge(in1, in2, in3);
	}
	char query;
	int src, dest;
	Pair<int, int> ret;
	Vertex* source;
	Vertex* destination;
	for (i = 0; i < q; ++i) {
		//쿼리 수행
		scanf(" %c %d %d", &query, &src, &dest);
		source = g.find_vertex(src);	//O(1)
		destination = g.find_vertex(dest);	//O(1)
		ret = dijkstra(g, tree, source, destination, i + 1, query);	//O(MlogN)
		if (ret.first == -1) {	//시작,도착 침수거나 거리 10^6 초과
			printf("None\n");
			continue;
		}
		switch (query)
		{
		case 'A':
			printf("%d %d %s %s\n", ret.second, ret.first, source->getname(), destination->getname());
			//src->dest 간의 최단거리 탐색
			break;
		case 'B':
			int size = ret.second;
			printf("%d ", size);
			int prev = dest;
			//treeset에 저장된 경로정보 뒤에서부터 탐색해 List head에 삽입, O(N)
			for (j = 0; j < size; ++j) {
				auto& p = tree.back();
				if (p.second == prev) {
					tree.push_front(p);
					prev = p.first;
				}
				tree.pop_back();
			}
			//TreeSet에 저장된 경로 출력, O(N)
			while (!tree.empty()) {
				auto& p = tree.front();
				printf("%d ", p.second);
				tree.pop_front();
			}
			printf("\n");
			//src->dest 간의 최단경로 탐색
			break;
		}
		//쿼리 당 최악 수행시간복잡도 O(N+MlogN)
	}
	return 0;
}