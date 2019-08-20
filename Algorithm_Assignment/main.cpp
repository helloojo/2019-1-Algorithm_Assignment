#include <stdio.h>

void m_strcpy(char* dest, char* src) {
	//���ڿ� ���� �Լ�
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
	STL list ����ϰ� ����
	Double Linked List
	��� ���� �ð����⵵ O(1)
	*/
private:
	//Linked List Node
	struct Node {
		Node* prev;
		Node* next;
		T data;
	};
	//���� �ӵ��� ���� NodePool
	static Node Pool[LIST_POOL_SIZE];
	//NodePool count ����
	static int cnt;
	//List�� ������ Node �ǹ�
	Node endNode;
public:
	//List Ž���� ���� ����� Iterator ����
	class Iterator {
	private:
		Node* point;	//List ��ġ ����
		inline Node* getpoint() const {
			return point;
		}
		friend class List<T>;	//List class�� friend class ����
	public:
		Iterator(Node* p = nullptr) : point(p) {}
		//iterator ������ �����ε�
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
	int list_size;	//list size ���� ����
	//NodePool���� Node������ �ʱ�ȭ �� ��ȯ
	Node* make_node(const T& data = T()) {
		Node* new_node = &Pool[cnt++];
		new_node->data = data;
		new_node->prev = new_node->next = nullptr;
		return new_node;
	}
public:
	/*
	List �ʱ�ȭ ������
	head,tail�� List�� ���� �ǹ��ϴ� endNode ����
	*/
	List() : endNode({ nullptr,nullptr,T() }), head(&endNode), tail(&endNode), list_size(0) {}
	//Head�� data Node �߰��ϴ� �Լ�
	void push_front(const T& data) {
		Node* new_node = make_node(data);
		new_node->next = head.getpoint();
		head.getpoint()->prev = new_node;
		head = new_node;	//head�� �߰��� ��� ����
		++list_size;
	}
	//tail�� data Node �߰��ϴ� �Լ�
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
		tail.getpoint()->prev = new_node;	//tail ���� Node�߰�
		new_node->next = tail.getpoint();
		++list_size;
	}
	//tail�� data Node �ϳ� �����ϴ� �Լ�
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
	//head�� data Node �ϳ� �����ϴ� �Լ�
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
	//head�� ���� ��ȯ �Լ�, const�� non-const �� ���� ����
	const T& front() const {
		if (!list_size) {
			throw "No Element";
		}
		return *head;
	}
	T& front() {
		return const_cast<T&>(static_cast<const List&>(*this).front());
	}
	//tail�� ���� ��ȯ �Լ�, const�� non-const �� ���� ����
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
	//head iterator ��ȯ �Լ�
	iterator& begin() {
		return head;
	}
	//tail iterator ��ȯ �Լ�
	iterator& end() {
		return tail;
	}
	//list_size ��ȯ �Լ�
	int size() const {
		return list_size;
	}
	//List empty���� ��ȯ �Լ�
	bool empty() const {
		return !list_size;
	}
};

//List�� Static ���� �ʱ�ȭ
template <class T>
int List<T>::cnt = 0;
template <class T>
typename List<T>::Node List<T>::Pool[LIST_POOL_SIZE];

//Edge, FringeSet�� ���� Vertex Class ����
class Vertex;

//Heap�� ���� FringeSet ����ü ����
struct FringeSet {
	int weight;	//d(src,i)+W(ij)����
	int prev;	//i Vertex�� key ����
	int herenum;//j Vertex�� key ����
	int order;	//Priority Queue�� insert�� ���� ����
	Vertex* here;//j Vertex�� ����Ű�� ������
};

/*
FringeSet �� ������ �����ε�
weight�� ���ϰ�
weight�� ������ j Vertex�� key�� ���ϰ�,
j Vertex�� key�� ������ Priority Queue�� ���Ե� ������ ���Ѵ�.
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
	minheap�� ����
	weight �ּҰ��� Top�� ��ġ
	*/
private:
	//�ִ� 400000�� Edge�� �����ϹǷ� Heapũ�� 400000���� ����
	FringeSet heap[400001];
	int pos;	//Heap�� Data�� ���Ե� ��ġ
	/*
	UpHeap ���� ����
	parent�� ���� ��,
	parent���� ������ parent���� upheap ����,
	ũ�� data ����
	�ð����⵵ O(logN)
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
	DownHeap ���� ����
	left, right child�� min�� ���� ��,
	data�� ���� minchild�� �� ũ�� ��ġ�� ����,
	������ minchild ��ġ���� downheap ����
	�ð����⵵ 2logN -> O(logN)
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
	//Priority_Queue �ʱ�ȭ, O(M)
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
	//Priority_Queue�� ���� ����, O(logN)
	void push(const FringeSet& data) {
		heap[pos++] = data;
		up_heap(pos - 1, data);
	}
	//Priority_Queue top(min value) ��ȯ
	FringeSet& top() {
		return heap[1];
	}
	//Priority_Queue Empty���� ��ȯ
	bool empty() const {
		return pos == 1;
	}
	//Priority_Queue�� ���� ����, O(logN)
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
	���� Vertex, �� Vertex, �Ÿ��� ����
	*/
private:
	Vertex* src;
	Vertex* dest;
	int distance;
public:
	Edge() : src(nullptr), dest(nullptr), distance(0) {}
	//Edge �ʱ�ȭ �Լ�
	void init(Vertex* s, Vertex* d, int dis) {
		src = s;
		dest = d;
		distance = dis;
	}
	//Distance ��ȯ �Լ�
	int getdistance() const {
		return distance;
	}
	//���� Vertex ��ȯ �Լ�
	Vertex* getsrc() const {
		return src;
	}
	//���� Vertex ��ȯ �Լ�
	Vertex* getdest() const {
		return dest;
	}
};
//EdgePool��, ������� Pool������ �ľ��ϱ� ���� count����
static Edge E_Pool[400001];
static int e_cnt = 0;

class Vertex {
	/*
	Vertex Class
	������ ������ �����ϴ� ��������Ʈ,
	Key, Vertex�̸�, ħ������, ����(Tree, Fringe or Unseen) ����
	*/
private:
	List<Edge*> adj_list;
	int key;	//������ȣ
	char name[21];	//�����̸�
	bool flooding;	//ħ������
	int state;
public:
	Vertex() : key(0), flooding(false), state(0) {}
	//Vertex �ʱ�ȭ �Լ�
	void init(int k, char* n, bool f) {
		m_strcpy(name, n);
		key = k;
		flooding = f;
	}
	//��������Ʈ�� Edge �߰�
	void addEdge(Edge* new_edge) {
		adj_list.push_back(new_edge);
	}
	//Vertex�� �̸� ��ȯ
	const char* getname() const {
		return name;
	}
	//Vertex�� Key ��ȯ
	int getdata() const {
		return key;
	}
	//Vertex�� ħ������ ��ȯ
	bool isflooding() const {
		return flooding;
	}
	//Vertex�� Treeset���� ��ȯ
	bool istree(int code) const {
		return state == code;
	}
	//Vertex�� Treeset���� ����
	void settree(int code) {
		state = code;
	}
	//Vertex�� incident�� Edge(��������Ʈ)��ȯ
	List<Edge*>& incident_edges() {
		return adj_list;
	}
};
//VertexPool��, ������� Pool������ �ľ��ϱ� ���� count����
static Vertex V_Pool[100001];
static int v_cnt = 0;

class Graph {
	/*
	Graph Class
	vertex�� Key������ Vertex�� ã�� �� �ִ� ������ �迭,
	���� �׷��� ���� ����
	*/
private:
	Vertex* vertex[1000000];
	bool digraph;
	Vertex* make_vertex(int key, char* name, bool flooding) {
		//Vertex Pool���� Vertex������ ������ �ʱ�ȭ �� �߰�, O(1)
		Vertex* new_vertex = &V_Pool[v_cnt++];
		new_vertex->init(key, name, flooding);
		vertex[key] = new_vertex;
		return new_vertex;
	}
	void make_edge(Vertex* src, Vertex* dest, int distance) {
		//Edge Pool���� Edge������ ������ �ʱ�ȭ �� �߰�, O(1)
		Edge* new_edge = &E_Pool[e_cnt++];
		new_edge->init(src, dest, distance);
		//src�� ��������Ʈ�� Edge �߰�, O(1)
		src->addEdge(new_edge);
	}
public:
	//digraph ������ true�ϰ�� ����׷���
	Graph(bool digraph = true) : digraph(digraph) {}
	//Vertex �߰��Լ�, O(1)
	void insert_vertex(int key, char* name, bool flooding) {
		make_vertex(key, name, flooding);
	}
	//Edge �߰��Լ�, O(1)
	void insert_edge(int s, int d, int distance) {
		//�������� Ž��, O(1)
		Vertex* src = find_vertex(s);
		//�������� Ž��, O(1)
		Vertex* dest = find_vertex(d);
		if (src->isflooding() || dest->isflooding()) {
			return;
		}
		//src->dest Edge ����, O(1)
		make_edge(src, dest, distance);
		//����׷����ϰ�� �ݴ� Edge�� �߰�, O(1)
		if (!digraph) {
			make_edge(dest, src, distance);
		}
	}
	//Vertex Ž���Լ�, O(1)
	Vertex* find_vertex(int key) {
		return vertex[key];
	}
};

//fringe set ����
Priority_Queue fringe;
Pair<int, int> dijkstra(Graph& g, List<Pair<int, int>>& tree, Vertex* src, Vertex* dest, int code, char query) {
	/*
	Dijkstra Algorithm ����
	query�� ���� ��θ� ����(B�϶� ����)
	��ȯ�� : Pair<int,int>
	first -> �ִ� �Ÿ�(10^6 �ʰ� �� -1)
	second -> TreeSet ũ��
	����, �������� �� ħ������ �����ϰų�, ��� �������� ���� ��
	{-1,-1} ��ȯ
	*/
	if (src->isflooding() || dest->isflooding()) {
		//����, �������� �� ħ�� ����
		return { -1,-1 };
	}
	register int order = 0;	//FringeSet�� �� ���� ����
	register int tree_size = 0;	//TreeSet�� ũ�� ����
	fringe.push({ 0,-1,src->getdata(),++order,src });	//�������� Fringe Set�� �߰�

	//Fringe Set�� ���� Vertex �ִ� M, FringeSet ���� ���⵵ O(logM)
	while (!fringe.empty()) {
		Vertex* s = fringe.top().here;	//d(src,i)+W(is)�� �ּҰ� �Ǵ� Vertex S, O(1)
		int here = fringe.top().herenum;
		int prev = fringe.top().prev;	//i Vertex�� Key, O(1)
		int dis = fringe.top().weight;	//d(src,i)+W(is), O(1)
		fringe.pop();		//fringeSet���� ����, O(logM), �� M*logM
		if (s->istree(code)) {	//S�� TreeSet�� �����ϸ� ������� ����
			continue;
		}
		s->settree(code);	//S�� TreeSet�� �߰�
		if (query == 'B') {	//��θ� �����ؾ��ϴ� Query�� ��� ����(����Vertex,����Vertex)
			tree.push_back({ prev,here });
		}
		++tree_size;	//treeset�� ũ������
		if (s == dest) {
			//S Vertex�� ���������ϰ��
			//fringe set �ʱ�ȭ �� �������������� �Ÿ��� Treeset ũ�� ��ȯ
			fringe.clear();
			return { (dis > 1000000 ? -1 : dis), tree_size };
		}
		//auto -> List<Edge*>&
		auto& adj_list = s->incident_edges();
		auto e = adj_list.end();
		//incident Edge�� ���� S�� ������ ���� ��� Ž��, O(deg(s)), �� sigma(s->V)(deg(s)) -> O(M)
		for (auto i = adj_list.begin(); i != e; ++i) {
			Vertex* next = i->getdest();	//���� Vertex n
			int weight = dis + i->getdistance(); //d(src,s)+W(sn)
			//Vertex n�� TreeSet�� �ƴҰ�� FringeSet�� �߰�, O(logM), �� O(MlogM)
			if (!next->istree(code)) {
				fringe.push({ weight,s->getdata(),next->getdata(),++order,next });
			}
		}
	}
	//�ð����⵵ O(MlogM)+O(MlogM) -> O(MlogM),(M<N^2) -> O(MlogN^2) -> O(2MlogN) -> O(MlogN)
	return { -1,-1 };
}

Graph g(false);	//�׷��� ����
List<Pair<int, int>> tree;	//��� �������� List ����

int main() {
	int n, m, q;
	char name[21] = "";
	int in1, in2, in3;
	register int i, j;
	scanf("%d %d %d", &n, &m, &q);
	//Vertex �߰�
	for (i = 0; i < n; ++i) {
		scanf("%d %s %d", &in1, name, &in3);
		g.insert_vertex(in1, name, in3);
	}
	//Edge �߰�
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
		//���� ����
		scanf(" %c %d %d", &query, &src, &dest);
		source = g.find_vertex(src);	//O(1)
		destination = g.find_vertex(dest);	//O(1)
		ret = dijkstra(g, tree, source, destination, i + 1, query);	//O(MlogN)
		if (ret.first == -1) {	//����,���� ħ���ų� �Ÿ� 10^6 �ʰ�
			printf("None\n");
			continue;
		}
		switch (query)
		{
		case 'A':
			printf("%d %d %s %s\n", ret.second, ret.first, source->getname(), destination->getname());
			//src->dest ���� �ִܰŸ� Ž��
			break;
		case 'B':
			int size = ret.second;
			printf("%d ", size);
			int prev = dest;
			//treeset�� ����� ������� �ڿ������� Ž���� List head�� ����, O(N)
			for (j = 0; j < size; ++j) {
				auto& p = tree.back();
				if (p.second == prev) {
					tree.push_front(p);
					prev = p.first;
				}
				tree.pop_back();
			}
			//TreeSet�� ����� ��� ���, O(N)
			while (!tree.empty()) {
				auto& p = tree.front();
				printf("%d ", p.second);
				tree.pop_front();
			}
			printf("\n");
			//src->dest ���� �ִܰ�� Ž��
			break;
		}
		//���� �� �־� ����ð����⵵ O(N+MlogN)
	}
	return 0;
}