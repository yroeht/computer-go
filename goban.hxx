#include <iostream>
#include <set>
#include <algorithm>
#include <assert.h>

#define LIBERTY_ADD     1
#define LIBERTY_REM     1

#define ORTHOGONAL_APPLY(Lambda, I, J) \
  do {                          \
      if (I > 0)                \
        Lambda(I - 1, J);       \
      if (J > 0)                \
        Lambda(I, J - 1);       \
      if (I + 1 < goban_size)   \
        Lambda(I + 1, J);       \
      if (J + 1 < goban_size)   \
        Lambda(I, J + 1);       \
  }                             \
  while (false);


inline
std::ostream& operator<<(std::ostream& os, const t_position& pos)
{
  char row = pos.first + 'A';
  if (row >= 'I')
    ++row;
  os << row << pos.second + 1;
  return os;
}

inline
Cell::Cell(unsigned short i, unsigned short j, t_color color_)
{
  if (Empty == (this->color = color_))
    this->group = nullptr;
  else
    {
      this->group = new t_group();
      this->group->stones.insert(t_position(i, j));
    }
}

inline
t_group*
Cell::get_group()
{
  return this->group;
}

inline
void
Cell::set_group(t_group* group_)
{
  this->group = group_;
}

template<unsigned short goban_size>
Cell&
Goban<goban_size>::cell(t_position stone)
{
  return board[stone.first][stone.second];
}

template<unsigned short goban_size>
Goban<goban_size>::Goban()
{
  for (unsigned short i = 0; i < goban_size; ++i)
    {
      board.push_back(*new std::vector<Cell>);
      for (unsigned short j = 0; j < goban_size; ++j)
        board.back().push_back(*new Cell(i, j, Empty));
    }
  ko = t_position(PASS, PASS);
  determine_hoshis();
}

template<unsigned short goban_size>
t_stones
Goban<goban_size>::get_liberties(unsigned short i,
                                 unsigned short j,
                                 t_color color)
{
  t_stones ret;
  auto process = [&](unsigned short x, unsigned short y)
    {
      auto& cell = board[x][y];
      if (cell.color == Empty)
        ret.insert(t_position(x, y));
      else if (cell.color == color)
        for (auto s : cell.get_group()->liberties)
          ret.insert(s);
    };
  ORTHOGONAL_APPLY(process, i, j);
  ret.erase(t_position(i, j));
  return ret;
}

template<unsigned short goban_size>
t_stones
Goban<goban_size>::get_liberties(unsigned short i, unsigned short j)
{
  return get_liberties(i, j, board[i][j].color);
}

template<unsigned short goban_size>
t_stones
Goban<goban_size>::get_liberties(const t_position pos)
{
  return get_liberties(pos.first, pos.second);
}

template<unsigned short goban_size>
void
Goban<goban_size>::add_strong_links(unsigned short i, unsigned short j)
{
  auto process = [&](unsigned short ii, unsigned short jj)
    {
      if ( board[ii][j].color == Empty
          && board[i][jj].color == Empty
          && board[ii][jj].color == board[i][j].color)
        strong_links.insert(t_strong_link(t_position(ii, j), t_position(i,jj)));
    };

  /* NW */
  if (i > 0 && j > 0)
    process(i-1, j-1);
  /* NE */
  if (i < goban_size-1 && j > 0)
    process(i+1, j-1);
  /* SW */
  if (i > 0 && j < goban_size-1)
    process(i-1, j+1);
  /* SE */
  if (i < goban_size-1 && j < goban_size-1)
    process(i+1, j+1);
}

template<unsigned short goban_size>
t_stones
Goban<goban_size>::get_neighbors(unsigned short i, unsigned short j)
{
  t_stones ret;

  auto process = [&](unsigned short x, unsigned short y)
    {
      if (board[x][y].color != Empty)
        ret.insert(t_position(x, y));
    };
  ORTHOGONAL_APPLY(process, i, j);
  for (auto s : ret)
    assert(cell(s).color != Empty);

  return ret;
}

template<unsigned short goban_size>
t_stones
Goban<goban_size>::get_neighbors(t_position pos)
{
  return get_neighbors(pos.first, pos.second);
}


template<unsigned short goban_size>
void
Goban<goban_size>::remove_stones(t_group* stones)
{
  for (auto stone : stones->stones)
    {
      for (auto neighbor : get_neighbors(stone.first, stone.second))
        this->cell(neighbor).get_group()->liberties.insert(stone);
      auto& cell = this->cell(stone);
      (cell.color == Black ? black_groups : white_groups).remove(stones);
      cell.color = Empty;
    }
}

template<unsigned short goban_size>
t_position
Goban<goban_size>::act_on_atari(t_color player)
{
  auto& playergroup = (player == Black ? black_groups : white_groups);
  auto& othergroup = (player != Black ? black_groups : white_groups);
  auto otherplayer = (player != Black ? Black : White);

  typedef std::pair<t_position, t_group*>   move;
  std::set<move>                            moves;

  for (auto g : othergroup)
    for (auto m : g->liberties)
      moves.insert(move(m, g));
  for (auto g : playergroup)
    for (auto m : g->liberties)
        moves.insert(move(m, g));

  for (auto m : moves)
    if (std::count_if(moves.begin(), moves.end(),
                      [=](move x) -> bool {return (x.first == m.first); })
        != 1)
      ; // this sucks balls. This shouldn't happen, but it does.
  potential_moves.clear();
  for (auto m : moves)
    {
      double oldlib = m.second->liberties.size();
      double newlib = (double)get_liberties(m.first.first,
                                            m.first.second, player).size();
      double newli2 = (double)get_liberties(m.first.first,
                                            m.first.second, otherplayer).size();

      double w =  2*LIBERTY_REM * (newli2 / oldlib);
      double w2 = LIBERTY_ADD * (newlib / oldlib);
      std::cerr << "move (" << m.first.first << ", " << m.first.second << ") w="
        << w << " w2=" << w2 << " oldlib=" << oldlib << " newlib=" << newlib << std::endl;
      w += w2;
      potential_moves.push_back(t_weighed_stone(m.first, w));
    }
  std::sort(potential_moves.begin(),
            potential_moves.end(),
            [](const t_weighed_stone& a,
               const t_weighed_stone& b) -> bool {
              return a.second < b.second;
              });

  /* 'pass' should be something like (0, 0) or (-1, -1), alas our
  ** representation is 0-based and unsigned.  */
  t_weighed_stone best_move(t_position(PASS, PASS), 1);
  for (auto m : potential_moves)
    if (m.second > best_move.second)
      best_move = m;
  std::cerr << "best move: " << best_move.first << std::endl;
  return best_move.first;
}

template<unsigned short goban_size>
bool
Goban<goban_size>::play(unsigned short i, unsigned short j, t_color c)
{
  assert(board[i][j].color == Empty);
  assert(c != Empty);
  if (ko.first == i && ko.second == j)
    return false;
  ko = t_position(PASS, PASS);

  auto& colorgroup = (c == Black ? black_groups : white_groups);
  board[i][j] = *new Cell(i, j, c);
  auto& new_stone = this->cell(t_position(i, j));
  new_stone.get_group()->liberties = get_liberties(i, j);
  colorgroup.push_front(new_stone.get_group());

  for (auto nei : get_neighbors(i, j))
    {
      auto &neighbor = this->cell(nei);
      auto neighbor_group = neighbor.get_group();
      if (neighbor.color == Empty) //already removed
        continue;
      else if (neighbor.color != c)
        {
          neighbor_group->liberties.erase(t_position(i, j));
          if (neighbor_group->liberties.size() == 0)
            {
              if (new_stone.get_group()->stones.size() == 1
                  && neighbor_group->stones.size() == 1)
                ko = *neighbor_group->stones.begin();
              remove_stones(neighbor_group);
            }
        }
      else
        {
          for (auto s : neighbor_group->stones)
            {
              new_stone.get_group()->stones.insert(s);
              this->cell(s).set_group(new_stone.get_group());
            }
          colorgroup.remove(neighbor_group);
        }
    }
  add_strong_links(i, j);
  return true;
}

template<unsigned short goban_size>
void
Goban<goban_size>::determine_hoshis()
{
  auto insert = [&](unsigned short i, unsigned short j, bool starter)
    {
      if (starter)
        starting_stones.insert(t_position(i, j));
      hoshis.insert(t_position(i, j));
    };

  if (goban_size == 13)
    {
      insert(3, 3, true);
      insert(3, 6, false);
      insert(3, 9, true);
      insert(6, 3, false);
      insert(6, 6, false);
      insert(6, 9, false);
      insert(9, 3, true);
      insert(9, 6, false);
      insert(9, 9, true);
    }
  else if (goban_size == 19)
    {
      insert(3, 3, true);
      insert(3, 9, false);
      insert(3, 15, true);
      insert(9, 3, false);
      insert(9, 9, false);
      insert(9, 15, false);
      insert(15, 3, true);
      insert(15, 9, false);
      insert(15, 15, true);
    }
  else
    std::cerr << "warning: goban has irregular size (" << goban_size
      << "), can't draw proper hoshi." << std::endl;
}

template<unsigned short goban_size>
void
Goban<goban_size>::dump()
{
  for (unsigned short i = 0; i < goban_size; ++i)
    {
      for (unsigned short j = 0; j < goban_size; ++j)
        {
          auto cell = board[i][j].color;
          if (Black == cell)
            std::cout << "B";
          else if (White == cell)
            std::cout << "W";
          else if (Empty == cell
                   && hoshis.end() != std::find(hoshis.begin(),
                                                hoshis.end(),
                                                t_position(i, j)))
            std::cout  << "*";
          else
            std::cout << ".";
          std::cout << " ";
        }
      std::cout << std::endl;
    }
  std::cout << std::endl;
}

template<unsigned short goban_size>
void
Goban<goban_size>::dump_groups()
{
  std::cout << "Begin black groups dump:" << std::endl;
  dump_group(black_groups);
  std::cout << "Begin white groups dump:" << std::endl;
  dump_group(white_groups);
  std::cout << "End groups dump." << std::endl;
}

template<unsigned short goban_size>
void
Goban<goban_size>::dump_group(t_groups groups)
{
  for (auto group : groups)
    {
      std::cout << " - " << group->liberties.size() << "L (" << group << ") ";
      for (auto cell : group->stones)
        std::cout << "(" << cell << this->cell(cell).get_group() << ") ";
      std::cout << std::endl;
      std::cout << "   lib: ";
      for (auto cell : group->liberties)
        std::cout << cell;
      std::cout << std::endl;
    }
}

template<unsigned short goban_size>
void
Goban<goban_size>::dump_links()
{
  std::cout << "Begin strong links dump:" << std::endl;
  for (auto link : strong_links)
    std::cout << " - " << link.first << " " << link.second << std::endl;
  std::cout << "End strong links dump." << std::endl;
}

template<unsigned short goban_size>
void
Goban<goban_size>::dump_moves()
{
  std::cerr << "Begin moves dump:" << std::endl;
  for (auto m : potential_moves)
    std::cerr << " - " << m.first << " " << m.second << std::endl;
  std::cerr << "End moves dump." << std::endl;

}

template<unsigned short goban_size>
void
Goban<goban_size>::reset()
{
  strong_links.clear();
  white_groups.clear();
  black_groups.clear();
  potential_moves.clear();

  for (unsigned short i = 0; i < goban_size; ++i)
    for (unsigned short j = 0; j < goban_size; ++j)
      {
        board[i][j].color = Empty;
        board[i][j].set_group(nullptr);
      }
  ko = t_position(PASS, PASS);
}
