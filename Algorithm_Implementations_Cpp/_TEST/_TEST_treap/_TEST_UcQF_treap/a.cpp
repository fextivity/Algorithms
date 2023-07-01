#include <x86intrin.h>
#include <bits/stdc++.h>
using namespace std;
#if __cplusplus > 201703L
#include <ranges>
using namespace numbers;
#endif

// DEBUG BEGIN
#ifdef LOCAL
template<class L, class R> ostream &operator<<(ostream &out, const pair<L, R> &p){
	return out << "{" << p.first << ", " << p.second << "}";
}
template<class Tuple, size_t N> struct _tuple_printer{
	static ostream &_print(ostream &out, const Tuple &t){ return _tuple_printer<Tuple, N-1>::_print(out, t) << ", " << get<N-1>(t); }
};
template<class Tuple> struct _tuple_printer<Tuple, 1>{
	static ostream &_print(ostream &out, const Tuple& t){ return out << get<0>(t); }
};
template<class... Args> ostream &_print_tuple(ostream &out, const tuple<Args...> &t){
	return _tuple_printer<decltype(t), sizeof...(Args)>::_print(out << "{", t) << "}";
}
template<class ...Args> ostream &operator<<(ostream &out, const tuple<Args...> &t){
	return _print_tuple(out, t);
}
template<class T> ostream &operator<<(class enable_if<!is_same<T, string>::value, ostream>::type &out, const T &arr){
	if(arr.empty()) return out << "{}";
	out << "{";
	for(auto it = arr.begin(); it != arr.end(); ++ it){
		out << *it;
		next(it) != arr.end() ? out << ", " : out << "}";
	}
	return out;
}
ostream &operator<<(ostream &out, const _Bit_reference &bit){
	return out << bool(bit);
}
template<class T, class A, class C>
ostream &operator<<(ostream &out, priority_queue<T, A, C> pq){
	vector<T> a;
	while(!pq.empty()) a.push_back(pq.top()), pq.pop();
	return out << a;
}
template<class Head>
void debug_out(Head H){ cerr << H << endl; }
template<class Head, class... Tail>
void debug_out(Head H, Tail... T){ cerr << H << ", ", debug_out(T...); }
void debug2_out(){ }
template<class Head, class... Tail>
void debug2_out(Head H, Tail... T){ cerr << "\n"; for(auto x: H) cerr << x << ",\n"; debug2_out(T...); }
template<class Width, class Head>
void debugbin_out(Width w, Head H){
	for(auto rep = w; rep; -- rep, H >>= 1) cerr << (H & 1);
	cerr << endl;
}
template<class Width, class Head, class... Tail>
void debugbin_out(Width w, Head H, Tail... T){
	for(auto rep = w; rep; -- rep, H >>= 1) cerr << (H & 1);
	cerr << ", "; debugbin_out(w, T...);
}
enum CODE{ CCRED = 31, CCGREEN = 32, CCYELLOW = 33, CCBLUE = 34, CCDEFAULT = 39 };
#define debug_endl() cerr << endl
#define debug(...) cerr << "\033[" << (int)CODE(CCRED) << "m[" << #__VA_ARGS__ << "]: \033[" << (int)CODE(CCBLUE) << "m", debug_out(__VA_ARGS__), cerr << "\33[" << (int)CODE(CCDEFAULT) << "m"
#define debug2(...) cerr << "\033[" << (int)CODE(CCRED) << "m[" << #__VA_ARGS__ << "] \033[" << (int)CODE(CCBLUE) << "m", debug2_out(__VA_ARGS__), cerr << "\33[" << (int)CODE(CCDEFAULT) << "m"
#define debugbin(...) cerr << "\033[" << (int)CODE(CCRED) << "m[" << #__VA_ARGS__ << "] \033[" << (int)CODE(CCBLUE) << "m", debugbin_out(__VA_ARGS__), cerr << "\33[" << (int)CODE(CCDEFAULT) << "m"
#else
#define debug_endl() 42
#define debug(...) 42
#define debug2(...) 42
#define debugbin(...) 42
#endif
// DEBUG END

template<bool HAS_QUERY, bool HAS_UPDATE, bool HAS_FLIP, bool IS_COMMUTATIVE, class T, class U, class F1, class F2, class F3, class F4>
struct treap_base{
#define ifQ if constexpr(HAS_QUERY)
#define ifU if constexpr(HAS_UPDATE)
#define ifF if constexpr(HAS_FLIP)
#define ifNC if constexpr(!IS_COMMUTATIVE)
	vector<unsigned int> priority;
	vector<int> pv, left, right, internal_size, lazy_flip;
	vector<T> data, subtr_data;
	vector<U> lazy;
	F1 TT; // monoid operation (always adjacent)
	T T_id; // monoid identity
	F2 UU; // monoid operation (superset, subset)
	U U_id; // monoid identity
	F3 UT; // action of U on T (superset, subset)
	F4 FlipT;
	treap_base(F1 TT, T T_id, F2 UU, U U_id, F3 UT, F4 FlipT = plus<>()): TT(TT), T_id(T_id), UU(UU), U_id(U_id), UT(UT), FlipT(FlipT){ }
	void push(int u){
		ifU if(lazy[u] != U_id){
			if(auto v = left[u]; ~v){
				data[v] = UT(lazy[u], data[v]);
				lazy[v]  = UU(lazy[u], lazy[v]);
				ifQ subtr_data[v] = UT(lazy[u], subtr_data[v]);
			}
			if(auto w = right[u]; ~w){
				data[w] = UT(lazy[u], data[w]);
				lazy[w]  = UU(lazy[u], lazy[w]);
				ifQ subtr_data[w] = UT(lazy[u], subtr_data[w]);
			}
			lazy[u] = U_id;
		}
		ifF if(lazy_flip[u]){
			if(auto v = left[u]; ~v){
				swap(left[v], right[v]);
				lazy_flip[v] ^= 1;
				ifQ ifNC subtr_data[v] = FlipT(subtr_data[v]);
			}
			if(auto w = right[u]; ~w){
				swap(left[w], right[w]);
				lazy_flip[w] ^= 1;
				ifQ ifNC subtr_data[w] = FlipT(subtr_data[w]);
			}
			lazy_flip[u] = false;
		}
	}
	template<bool UPDATE_STRUCTURE = true>
	void refresh(int u){
		if constexpr(UPDATE_STRUCTURE) internal_size[u] = 1;
		ifQ{
			subtr_data[u] = T_id;
			if(auto v = left[u]; ~v){
				if constexpr(UPDATE_STRUCTURE){
					pv[v] = u;
					internal_size[u] += internal_size[v];
				}
				auto x = subtr_data[v];
				ifU if(lazy[u] != U_id) x = UT(lazy[u], x);
				ifF ifNC if(lazy_flip[u]) x = FlipT(x);
				subtr_data[u] = x;
			}
			subtr_data[u] = TT(subtr_data[u], data[u]);
			if(auto w = right[u]; ~w){
				if constexpr(UPDATE_STRUCTURE){
					pv[w] = u;
					internal_size[u] += internal_size[w];
				}
				auto x = subtr_data[w];
				ifU if(lazy[u] != U_id) x = UT(lazy[u], x);
				ifF ifNC if(lazy_flip[u]) x = FlipT(x);
				subtr_data[u] = TT(subtr_data[u], x);
			}
		}
	}
	void heapify(int u){
		if(~u){
			int v = u;
			if(~left[u] && priority[left[u]] > priority[v]) v = left[u];
			if(~right[u] && priority[right[u]] > priority[v]) v = right[u];
			if(u != v) swap(priority[u], priority[v]), heapify(v);
		}
	}
	template<class output_stream>
	output_stream &print(output_stream &out, int root){
		#ifdef LOCAL
		out << "[";
		traverse(root, [&](int root){ out << data[root] << ", "; });
		out << "\b\b]\n";
		#endif
		return out;
	}
	vector<int> dead_node;
	int new_node(const T &x, int v = -1, int w = -1){
		// Among all seeds in range [0, 1769666],
		// 72718 has the longest sequence of unique numbers of length 362129.
		static mt19937 rng(72718);
		int u;
		if(dead_node.empty()){
			u = (int)data.size();
			pv.push_back(-1);
			left.push_back(v);
			right.push_back(w);
			priority.push_back(rng());
			internal_size.push_back(1);
			data.push_back(x);
			ifQ subtr_data.push_back(x);
			ifU lazy.push_back(U_id);
			ifF lazy_flip.push_back(0);
		}
		else{
			u = dead_node.back();
			dead_node.pop_back();
			pv[u] = -1;
			left[u] = v;
			right[u] = w;
			priority[u] = rng();
			internal_size[u] = 1;
			data[u] = x;
			ifQ subtr_data[u] = x;
			ifU lazy[u] = U_id;
			ifF lazy_flip[u] = 0;
		}
		heapify(u);
		refresh(u);
		return u;
	}
	// O(1)
	int size(int root) const{
		return ~root ? internal_size[root] : 0;
	}
	// Split to [-inf, x), [x, inf)
	// Data must be sorted <
	// O(log(n))
	pair<int, int> split_by_key(int root, const T &x){
		if(!~root) return {-1, -1};
		push(root);
		if(data[root] < x){
			auto [a, b] = split_by_key(right[root], x);
			right[root] = a, refresh(root);
			return {root, b};
		}
		else{
			auto [a, b] = split_by_key(left[root], x);
			left[root] = b, refresh(root);
			return {a, root};
		}
	}
	// Split to [-inf, x), [x, inf)
	// Data must be sorted cmp
	// O(log(n))
	template<class Compare>
	pair<int, int> split_by_key(int root, const T &x, Compare cmp){
		if(!~root) return {-1, -1};
		push(root);
		if(cmp(data[root], x)){
			auto [a, b] = split_by_key(right[root], x, cmp);
			right[root] = a, refresh(root);
			return {root, b};
		}
		else{
			auto [a, b] = split_by_key(left[root], x, cmp);
			left[root] = b, refresh(root);
			return {a, root};
		}
	}
	// Split into [0, pos), [pos, size(root))
	// O(log(n))
	pair<int, int> split_by_order(int root, int pos){
		if(!~root){
			assert(pos == 0);
			return {-1, -1};
		}
		push(root);
		if(size(left[root]) < pos){
			auto [a, b] = split_by_order(right[root], pos - size(left[root]) - 1);
			right[root] = a, refresh(root);
			return {root, b};
		}
		else{
			auto [a, b] = split_by_order(left[root], pos);
			left[root] = b, refresh(root);
			return {a, root};
		}
	}
	// Split to [true Segment], [false Segment]
	// Data must be sorted by pred
	// O(log(n))
	pair<int, int> split_by_pred(int root, auto pred){
		if(!~root) return {-1, -1};
		push(root);
		if(pred(root)){
			auto [a, b] = split_by_pred(right[root], pred);
			right[root] = a, refresh(root);
			return {root, b};
		}
		else{
			auto [a, b] = split_by_pred(left[root], pred);
			left[root] = b, refresh(root);
			return {a, root};
		}
	}
	// Split into [0, l), [l, r), [r, size(root))
	// O(log(n))
	array<int, 3> split_to_three(int u, int l, int r){
		assert(0 <= l && l <= r && r <= size(u));
		if(!~u) return {-1, -1, -1};
		int a, b, c;
		tie(a, b) = split_by_order(u, l);
		tie(b, c) = split_by_order(b, r - l);
		return {a, b, c};
	}
	// Split into [0, pos[0]), [pos[0], pos[1]), ..., [pos[size(pos) - 1], size(root))
	// O(k log n)
	vector<int> split(int root, const vector<int> &pos){
		assert(is_sorted(pos.begin(), pos.end()));
		if(pos.empty()) return {root};
		assert(0 <= pos.front() && pos.back() <= size(root));
		if(!~root) return vector<int>((int)pos.size() + 1, -1);
		vector<int> res((int)pos.size() + 1);
		res[0] = root;
		for(auto i = 0, last = 0; i < (int)pos.size(); ++ i){
			tie(res[i], res[i + 1]) = split_by_order(res[i], pos[i] - last);
			last = pos[i];
		}
		return res;
	}
	// Append u and v
	// O(log(n))
	int append(int u, int v){
		if(!~u || !~v) return ~u ? u : v;
		push(u);
		push(v);
		if(priority[v] < priority[u]){
			right[u] = append(right[u], v), refresh(u);
			return u;
		}
		else{
			left[v] = append(u, left[v]), refresh(v);
			return v;
		}
	}
	// Append treaps in order
	// O((list length) * log(n))
	int append(const vector<int> &list){
		return accumulate(list.begin(), list.end(), -1, [&](int u, int v){ return append(u, v); });
	}
	// Data must be sorted by <
	// O(log(n))
	int _insert_by_key(int root, int u){
		if(!~root) return u;
		push(root);
		if(priority[root] > priority[u]){
			if(data[root] < data[u]) right[root] = _insert_by_key(right[root], u);
			else left[root] = _insert_by_key(left[root], u);
			refresh(root);
			return root;
		}
		auto [a, b] = split_by_key(root, data[u]);
		left[u] = a, right[u] = b;
		refresh(u);
		return u;
	}
	// Data must be sorted by <
	// O(log(n))
	int insert_by_key(int root, const T &x){
		return _insert_by_key(root, new_node(x));
	}
	// Data must be sorted by cmp
	// O(log(n))
	template<class Compare>
	int _insert_by_key(int root, int u, Compare cmp){
		if(!~root) return u;
		push(root);
		if(priority[root] > priority[u]){
			if(cmp(data[root], data[u])) right[root] = _insert_by_key(right[root], u, cmp);
			else left[root] = _insert_by_key(left[root], u, cmp);
			refresh(root);
			return root;
		}
		auto [a, b] = split_by_key(root, data[u], cmp);
		left[u] = a, right[u] = b;
		refresh(u);
		return u;
	}
	// Data must be sorted by cmp
	// O(log(n))
	template<class Compare>
	int insert_by_key(int root, const T &x, Compare cmp){
		return _insert_by_key(root, new_node(x), cmp);
	}
	// O(log(n))
	int _insert_by_order(int root, int pos, int u){
		if(!~root){
			assert(pos == 0);
			return u;
		}
		push(root);
		if(priority[root] > priority[u]){
			if(size(left[root]) < pos) right[root] = _insert_by_order(right[root], pos - size(left[root]) - 1, u);
			else left[root] = _insert_by_order(left[root], pos, u);
			refresh(root);
			return root;
		}
		auto [a, b] = split_by_order(root, pos);
		left[u] = a, right[u] = b;
		refresh(u);
		return u;
	}
	// O(log(n))
	int insert_by_order(int root, int pos, const T &x){
		return _insert_by_order(root, pos, new_node(x));
	}
	// Data must be sorted by pred
	// O(log(n))
	int _insert_by_pred(int root, auto pred, int u){
		if(!~root) return u;
		push(root);
		if(priority[root] > priority[u]){
			if(pred(data[root])) right[root] = _insert_by_pred(right[root], pred, u);
			else left[root] = _insert_by_pred(left[root], pred, u);
			refresh(root);
			return root;
		}
		auto [a, b] = split_by_pred(root, pred);
		left[u] = a, right[u] = b;
		refresh(u);
		return u;
	}
	// O(log(n))
	int insert_by_pred(int root, auto pred, const T &x){
		return _insert_by_pred(root, pred, new_node(x));
	}
	// Erase the smallest element >= x.
	// Return -2 when no such element exists
	// Data must be sorted by <
	// O(log(n))
	int erase_by_key(int root, const T &x){
		if(!~root) return -2;
		push(root);
		if(data[root] < x){
			int u = erase_by_key(right[root], x);
			if(u == -2) return -2;
			right[root] = u;
		}
		else{
			int u = erase_by_key(left[root], x);
			if(u == -2){
				dead_node.push_back(root);
				return append(left[root], right[root]);
			}
			left[root] = u;
		}
		refresh(root);
		return root;
	}
	// Erase the smallest element >= x.
	// Return -2 when no such element exists
	// Data must be sorted by cmp
	// O(log(n))
	template<class Compare>
	int erase_by_key(int root, const T &x, Compare cmp){
		if(!~root) return -2;
		push(root);
		if(cmp(data[root], x)){
			int u = erase_by_key(right[root], x, cmp);
			if(u == -2) return -2;
			right[root] = u;
		}
		else{
			int u = erase_by_key(left[root], x, cmp);
			if(u == -2){
				dead_node.push_back(root);
				return append(left[root], right[root]);
			}
			left[root] = u;
		}
		refresh(root);
		return root;
	}
	// O(log(n))
	int erase_by_order(int root, int pos){
		assert(~root);
		push(root);
		if(size(left[root]) == pos){
			dead_node.push_back(root);
			return append(left[root], right[root]);
		}
		if(size(left[root]) < pos) right[root] = erase_by_order(right[root], pos - size(left[root]) - 1);
		else left[root] = erase_by_order(left[root], pos);
		refresh(root);
		return root;
	}
	// Erase the smallest element x with !pred(x)
	// Return -2 when no such element exists
	// Data must be sorted by pred
	// O(log(n))
	int erase_by_pred(int root, auto pred){
		if(!~root) return -2;
		push(root);
		if(pred(data[root])){
			int u = erase_by_pred(right[root], pred);
			if(u == -2) return -2;
			right[root] = u;
		}
		else{
			int u = erase_by_pred(left[root], pred);
			if(u == -2){
				dead_node.push_back(root);
				return append(left[root], right[root]);
			}
			left[root] = u;
		}
		refresh(root);
		return root;
	}
	// Data must be sorted by <
	// O(min(size(u), size(v)) * log(size ratio))
	int unite_by_key(int u, int v){
		if(!~u || !~v) return ~u ? u : v;
		if(priority[u] < priority[v]) swap(u, v);
		auto [a, b] = split_by_key(v, data[u]);
		push(u);
		left[u] = unite_by_key(left[u], a);
		right[u] = unite_by_key(right[u], b);
		refresh(u);
		return u;
	}
	// Data must be sorted by cmp
	// O(min(size(u), size(v)) * log(size ratio))
	template<class Compare>
	int unite_by_key(int u, int v, Compare cmp){
		if(!~u || !~v) return ~u ? u : v;
		if(priority[u] < priority[v]) swap(u, v);
		auto [a, b] = split_by_key(v, data[u], cmp);
		push(u);
		left[u] = unite_by_key(left[u], a, cmp);
		right[u] = unite_by_key(right[u], b, cmp);
		refresh(u);
		return u;
	}
	void traverse(int root, auto f){
		if(~root){
			push(root);
			traverse(left[root], f);
			f(root);
			traverse(right[root], f);
			refresh(root);
		}
	}
	int build(int n){
		return build(vector<T>(n, T_id));
	}
	int build(int n, T init){
		return build(vector<T>(n, init));
	}
	int build(const vector<T> &a){
		auto recurse = [&](auto self, int l, int r)->int{
			if(l == r) return -1;
			int m = l + (r - l >> 1);
			return new_node(a[m], self(self, l, m), self(self, m + 1, r));
		};
		return recurse(recurse, 0, (int)a.size());
	}
	// Data must be sorted by <
	// O(log(n))
	int order_of_key(int root, const T &x){
		int res = 0;
		while(~root){
			push(root);
			if(data[root] < x){
				res += size(left[root]) + 1;
				root = right[root];
			}
			else root = left[root];
		}
		return res;
	}
	// Data must be sorted by cmp
	// O(log(n))
	template<class Compare>
	int order_of_key(int root, const T &x, Compare cmp){
		int res = 0;
		while(~root){
			push(root);
			if(cmp(data[root], x)){
				res += size(left[root]) + 1;
				root = right[root];
			}
			else root = left[root];
		}
		return res;
	}
	// Data must be sorted by pred
	// O(log(n))
	int partition_point(int root, auto pred){
		int res = 0;
		while(~root){
			push(root);
			if(pred(data[root])){
				res += size(left[root]) + 1;
				root = right[root];
			}
			else root = left[root];
		}
		return res;
	}
	// O(log(n))
	void set(int root, int p, const T &x){
		assert(0 <= p && p < size(root));
		int u = root;
		while(true){
			push(u);
			if(size(left[u]) == p){
				data[u] = x;
				refresh<false>(u);
				break;
			}
			if(size(left[u]) > p) u = left[u];
			else{
				p -= size(left[u]) + 1;
				u = right[u];
			}
		}
		while(u != root){
			u = pv[u];
			refresh<false>(u);
		}
	}
	// O(log(n))
	T query(int root, int p){
		static_assert(HAS_QUERY);
		assert(0 <= p && p < size(root));
		while(true){
			push(root);
			if(size(left[root]) == p) return data[root];
			if(size(left[root]) > p) root = left[root];
			else{
				p -= size(left[root]) + 1;
				root = right[root];
			}
		}
	}
	T _query(int root, int ql, int qr){
		static_assert(HAS_QUERY);
		if(!~root || qr <= 0 || size(root) <= ql) return T_id;
		if(ql <= 0 && size(root) <= qr) return subtr_data[root];
		push(root);
		T res = T_id;
		if(ql < size(left[root])) res = _query(left[root], ql, qr);
		if(ql <= size(left[root]) && size(left[root]) + 1 <= qr) res = TT(res, data[root]);
		if(size(left[root]) + 1 < qr) res = TT(res, _query(right[root], ql - size(left[root]) - 1, qr - size(left[root]) - 1));
		return res;
	}
	// O(log(n))
	T query(int root, int ql, int qr){
		static_assert(HAS_QUERY);
		assert(0 <= ql && ql <= qr && qr <= size(root));
		return ql == qr ? T_id : _query(root, ql, qr);
	}
	// O(log(n))
	void update(int root, int p, const U &f){
		static_assert(HAS_UPDATE);
		assert(0 <= p && p < size(root));
		int u = root;
		while(true){
			push(u);
			if(size(left[u]) == p){
				data[u] = UT(f, data[u]);
				refresh<false>(u);
				break;
			}
			if(size(left[u]) > p) u = left[u];
			else{
				p -= size(left[u]) + 1;
				u = right[u];
			}
		}
		while(u != root){
			u = pv[u];
			refresh<false>(u);
		}
	}
	// O(log(n))
	void _update(int root, int ql, int qr, const U &f){
		static_assert(HAS_UPDATE);
		if(!~root || qr <= 0 || size(root) <= ql) return;
		if(ql <= 0 && size(root) <= qr){
			data[root] = UT(f, data[root]);
			lazy[root] = UU(f, lazy[root]);
			refresh<false>(root);
			return;
		}
		push(root);
		if(ql < size(left[root])) _update(left[root], ql, qr, f);
		if(ql <= size(left[root]) && size(left[root]) + 1 <= qr) data[root] = UT(f, data[root]);
		if(size(left[root]) + 1 < qr) _update(right[root], ql - size(left[root]) - 1, qr - size(left[root]) - 1, f);
		refresh<false>(root);
	}
	// O(log(n))
	void update(int root, int ql, int qr, const U &f){
		static_assert(HAS_UPDATE);
		assert(0 <= ql && ql <= qr && qr <= size(root));
		if(ql == qr) return;
		_update(root, ql, qr, f);
	}
	// O(log(n))
	void _flip(int root, int ql, int qr){
		static_assert(HAS_FLIP);
		push(root);
		if(qr <= size(left[root])){
			_flip(left[root], ql, qr);
			refresh<false>(root);
		}
		else if(size(left[root]) + 1 <= ql){
			_flip(right[root], ql - size(left[root]) - 1, qr - size(left[root]) - 1);
			refresh<false>(root);
		}
		else{
			auto [ar, br] = split_by_order(right[root], qr - size(left[root]) - 1);
			auto [al, bl] = split_by_order(left[root], ql);
			if(~bl){
				push(bl);
				swap(left[bl], right[bl]);
				lazy_flip[bl] ^= 1;
				refresh<false>(bl);
			}
			if(~ar){
				push(ar);
				swap(left[ar], right[ar]);
				lazy_flip[ar] ^= 1;
				refresh<false>(ar);
			}
			left[root] = append(al, ar);
			right[root] = append(bl, br);
			refresh(root);
		}
	}
	// O(log(n))
	void flip(int root, int ql, int qr){
		static_assert(HAS_FLIP);
		assert(0 <= ql && ql <= qr && qr <= size(root));
		if(!~root || qr - ql <= 1) return;
		_flip(root, ql, qr);
	}
	// pred(sum[ql, r)) is T, T, ..., T, F, F, ..., F
	// Returns max r with T
	// O(log(n))
	int max_pref(int u, int ql, auto pred){
		static_assert(HAS_QUERY);
		int n = size(u);
		assert(0 <= ql && ql <= n && pred(T_id));
		if(ql == n) return n;
		T sum = T_id;
		auto recurse = [&](auto self, int u, int l)->int{
			int r = l + size(u);
			if(!~u || r <= ql) return n;
			if(ql <= l && pred(TT(sum, subtr_data[u]))){
				sum = TT(sum, subtr_data[u]);
				return n;
			}
			push(u);
			if(auto p = self(self, left[u], l); p < n) return p;
			l += size(left[u]);
			return ql <= l && !pred(sum = TT(sum, data[u])) ? l : self(self, right[u], l + 1);
		};
		return recurse(recurse, u, 0);
	}
	// pred(sum[l, qr)) is F, F, ..., F, T, T, ..., T
	// Returns min l with T
	// O(log(n))
	int max_suff(int u, int qr, auto pred){
		static_assert(HAS_QUERY);
		int n = size(u);
		assert(0 <= qr && qr <= n && pred(T_id));
		if(qr == 0) return 0;
		T sum = T_id;
		auto recurse = [&](auto self, int u, int r)->int{
			int l = r - size(u);
			if(!~u || qr <= l) return 0;
			if(r <= qr && pred(TT(subtr_data[u], sum))){
				sum = TT(subtr_data[u], sum);
				return 0;
			}
			push(u);
			if(auto p = self(self, right[u], r); p > 0) return p;
			r -= size(right[u]);
			return r <= qr && !pred(sum = TT(data[u], sum)) ? r : self(self, left[u], r - 1);
		};
		return recurse(recurse, u, n);
	}
#undef ifQ
#undef ifU
#undef ifF
#undef ifNC
};

template<class T>
auto make_treap(){
	using U = int;
	auto _TT = [&](T, T)->T{ return T{}; };
	auto _UU = [&](U, U)->U{ return U{}; };
	auto _UT = [&](U, T)->T{ return T{}; };
	auto _FlipT = [&](T)->T{ return T{}; };
	return treap_base<false, false, false, false, T, U, decltype(_TT), decltype(_UU), decltype(_UT), decltype(_FlipT)>(_TT, T{}, _UU, U{}, _UT, _FlipT);
}
// Supports query
template<class T, class F1>
auto make_Q_treap(F1 TT, T T_id){
	using U = int;
	auto _UU = [&](U, U)->U{ return U{}; };
	auto _UT = [&](U, T)->T{ return T{}; };
	auto _FlipT = [&](T)->T{ return T{}; };
	return treap_base<true, false, false, false, T, U, F1, decltype(_UU), decltype(_UT), decltype(_FlipT)>(TT, T_id, _UU, U{}, _UT, _FlipT);
}
// Supports update, commutative query, and flip
template<class T, class U, class F1, class F2, class F3>
auto make_UcQF_treap(F1 TT, T T_id, F2 UU, U U_id, F3 UT){
	auto _FlipT = [&](T)->T{ return T{}; };
	return treap_base<true, true, true, true, T, U, F1, F2, F3, decltype(_FlipT)>(TT, T_id, UU, U_id, UT, _FlipT);
}
// Supports update, query, and flip
template<class T, class U, class F1, class F2, class F3, class F4>
auto make_UQF_treap(F1 TT, T T_id, F2 UU, U U_id, F3 UT, F4 FlipT){
	return treap_base<true, true, true, false, T, U, F1, F2, F3, F4>(TT, T_id, UU, U_id, UT, FlipT);
}

int main(){
	cin.tie(0)->sync_with_stdio(0);
	cin.exceptions(ios::badbit | ios::failbit);
	int qn;
	cin >> qn;
	auto treap = make_UcQF_treap([&](long long x, long long y){ return max(x, y); }, 0LL, plus<>(), 0LL, plus<>());
	int root = -1;
	for(auto qi = 0; qi < qn; ++ qi){
		int type;
		cin >> type;
		if(type == 0){ // insert
			int p, x;
			cin >> p >> x;
			debug("insert", p, x);
			assert(0 <= p && p <= treap.size(root));
			root = treap.insert_by_order(root, p, x);
		}
		else if(type == 1){ // erase
			int p;
			cin >> p;
			assert(0 <= p && p < treap.size(root));
			debug("erase", p);
			root = treap.erase_by_order(root, p);
		}
		else if(type == 2){ // set
			int p, x;
			cin >> p >> x;
			assert(0 <= p && p < treap.size(root));
			debug("set", p, x);
			treap.set(root, p, x);
		}
		else if(type == 3){ // point update
			int p, f;
			cin >> p >> f;
			assert(0 <= p && p < treap.size(root));
			debug("point update", p, f);
			treap.update(root, p, f);
		}
		else if(type == 4){ // range update
			int l, r, f;
			cin >> l >> r >> f;
			assert(0 <= l && l <= r && r <= treap.size(root));
			debug("range update", l, r, f);
			treap.update(root, l, r, f);
		}
		else if(type == 5){ // point query
			int p;
			cin >> p;
			assert(0 <= p && p < treap.size(root));
			debug("point query", p);
			cout << treap.query(root, p) << "\n";
		}
		else if(type == 6){ // range query
			int l, r;
			cin >> l >> r;
			assert(0 <= l && l <= r && r <= treap.size(root));
			debug("range query", l, r);
			cout << treap.query(root, l, r) << "\n";
		}
		else{ // range flip
			int l, r;
			cin >> l >> r;
			assert(0 <= l && l <= r && r <= treap.size(root));
			debug("range flip", l, r);
			treap.flip(root, l, r);
		}
	}
	return 0;
}

/*

*/

////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                                   Coded by Aeren                                   //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////