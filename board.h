#include <array>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>

#define INFTY 2147483647

int abs(int n) { return n > 0 ? n : -n; }
int sort_countInv(std::vector<int> &para, std::vector<int>::iterator begin, std::vector<int>::iterator end)
{
  if(end == begin + 1)
    return 0;
  auto middle = begin + int((end - begin)/2);
  int num_inversion = sort_countInv(para, begin, middle) + sort_countInv(para, middle, end);
  std::vector<int> front(begin, middle), behind(middle, end);
  front.push_back(INFTY);
  behind.push_back(INFTY);
  auto it_f = front.begin(), it_b = behind.begin();
  while(begin != end)
  {
    if(*it_b < *it_f)
    {
      *begin++ = *it_b++;
      num_inversion += front.end() - it_f - 1;
    }
    else
      *begin++ = *it_f++;
  }
  return num_inversion;
}

class Node
{
  friend class Board;
public:
  using state = std::vector<int>;
  Node() : s(0), g(0), h(0), f(0) {}
  Node(const state &state, int x, int y, std::shared_ptr<Node> n = nullptr) :
      s(state), g(x), h(y), f(x+y), prev(n) {}
  Node(const state::iterator b, const state::iterator e, int x, int y, std::shared_ptr<Node> n = nullptr) :
      s(b, e), g(x), h(y), f(x+y), prev(n) {}

private:
  state s;
  int g, h, f;
  std::shared_ptr<Node> prev;
};

class Board
{
  friend bool isSovable(const Board&);

public:
  using state = Node::state;
  using action = int;
  Board(state s) : root(std::make_shared<Node>(s, 0, heuristic(s))) {}
  void search();
  void print(std::ostream&);
  const state& getInit() const { return root->s; }

private:
  std::shared_ptr<Node> root;
  std::shared_ptr<Node> curr;
  void expand(std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>&) const;
  state result(const state&, int) const;
  std::vector<action> actions(const state&) const;
  bool goalTest(const state&) const;
  int heuristic(const state&) const;
  size_t findNum(size_t, const state &) const;
  void printSate(std::ostream &, const state&) const;
  std::pair<std::shared_ptr<Node>, int> RBFS(std::shared_ptr<Node>, int) const;
};

// friend
bool isSovable(const Board& board)
{
  std::vector<int> para;
  for (auto it = board.getInit().begin(), end = board.getInit().end(); it != end; ++it)
  {
    if(*it != 0)
      para.push_back(*it);
  }
  int num_inversion = sort_countInv(para, para.begin(), para.end());
  return !(num_inversion%2);
}

//public
void Board::search()
{
  if (!curr)
  {
    if(isSovable(root->s))
      curr = RBFS(root, INFTY).first;
    else
      curr = std::make_shared<Node>();
  }
}
void Board::print(std::ostream &os)
{
  if (!curr)
  {
    os << "not yet research." << std::endl;
    return;
  }
  if(curr->s.empty())
  {
    os << "this board is unsolvable." << std::endl;
    return;
  }
  auto n(curr);
  std::vector<std::shared_ptr<Node>> seq{curr};
  while(n->prev)
  {
    seq.push_back(n->prev);
    n = n->prev;
  }
  for (auto it = seq.rbegin(), end = seq.rend(); it != end; ++it)
  {
    printSate(os, (*it)->s);
    os << std::endl;
  }
}

//private
void Board::expand(std::shared_ptr<Node> n, std::vector<std::shared_ptr<Node>> &v) const
{
  for (auto a : actions(n->s))
  {
    state newS = result(n->s, a);
    v.push_back(std::make_shared<Node>(newS, n->g + 1, heuristic(newS), n));
    std::push_heap(v.begin(), v.end(),
                   [](auto a, auto b) { return a->f > b->f; });
  }
}
std::vector<Board::action> Board::actions(const state &s) const
{
  std::vector<action> ret;
  int pos_blank(findNum(0, s));
  if(pos_blank > 2)
    ret.push_back(0);
  if(pos_blank%3 - 2)
    ret.push_back(1);
  if(pos_blank < 6)
    ret.push_back(2);
  if(pos_blank%3)
    ret.push_back(3);
  return ret;
}
size_t Board::findNum(size_t n, const state &s) const
{
  for (size_t i = 0; i < 9; ++i)
    if (s[i] == n) return i;
  return -1;
}
int Board::heuristic(const state &s) const
{
  int manhattan_dist(0);
  for (size_t i = 1; i < 9; ++i)
  {
    size_t pos = findNum(i, s);
    manhattan_dist += abs((int(i - 1 - pos) / 3)) + abs(int((i - 1) % 3 - pos % 3));
  }
  return manhattan_dist;
}
Board::state Board::result(const state &s, int direction) const
{
  state trans_state(s);
  size_t pos_swap, pos_balnk = findNum(0, trans_state);
  if (direction == 0 && pos_balnk > 2)
    pos_swap = pos_balnk - 3;
  else if (direction == 2 && pos_balnk < 6)
    pos_swap = pos_balnk + 3;
  else if (direction == 1 && pos_balnk%3 - 2)
    pos_swap = pos_balnk + 1;
  else if (direction == 3 && pos_balnk%3)
    pos_swap = pos_balnk - 1;
  std::swap(trans_state[pos_balnk], trans_state[pos_swap]);
  return trans_state;
}
bool Board::goalTest(const state &s) const
{
  for (size_t i = 1; i < 9; i ++)
    if (findNum(i, s) != i-1)
      return false;
  return true;
}
void Board::printSate(std::ostream &os, const state &s) const
{
  for (size_t i = 0; i < 9; ++i)
  {
    if (i%3)
      os << " ";
    else 
      os << std::endl;
    os << s[i];
  }
}
std::pair<std::shared_ptr<Node>, int> Board::RBFS(std::shared_ptr<Node> n, int f_limit) const
{
  if (goalTest(n->s))
    return std::make_pair(n, 0);
  std::vector<std::shared_ptr<Node>> successors;
  expand(n, successors);
  if(successors.empty())
    return std::make_pair(nullptr, INFTY);
  for (auto s : successors)
    s->f = std::max(s->f, n->f);
  
  while(true)
  {
    std::pop_heap(successors.begin(), successors.end());
    auto best(successors.back());
    successors.pop_back();
    if(best->f > f_limit)
      return std::make_pair(nullptr, INFTY);

    std::pop_heap(successors.begin(), successors.end());
    int alt(successors.back()->f);
    std::push_heap(successors.begin(), successors.end());

    auto p(RBFS(best, std::min(f_limit, alt)));
    if (p.first)
      return std::make_pair(p.first, 0);
    best->f = p.second;

    successors.push_back(best);
    std::push_heap(successors.begin(), successors.end());
  }
}
