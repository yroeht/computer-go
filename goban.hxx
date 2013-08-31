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
Cell::Cell(unsigned int i, unsigned int j, t_color color)
{
  this->color = color;
  if (color == Empty)
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
Cell::set_group(t_group* group)
{
  this->group = group;
}

template<int goban_size>
Cell&
Goban<goban_size>::cell(t_position stone)
{
  return board[stone.first][stone.second];
}

template<int goban_size>
Goban<goban_size>::Goban()
{
  for (auto i = 0; i < goban_size; ++i)
    {
      board.push_back(*new std::vector<Cell>);
      for (auto j = 0; j < goban_size; ++j)
        board.back().push_back(*new Cell(i, j, Empty));
    }
}

template<int goban_size>
t_stones
Goban<goban_size>::get_liberties(unsigned int i, unsigned int j, t_color color)
{
  t_stones ret;
  auto process = [&](unsigned int i, unsigned int j)
    {
      auto& cell = board[i][j];
      if (cell.color == Empty)
        ret.insert(t_position(i, j));
      else if (cell.color == color)
        for (auto s : cell.get_group()->liberties)
          ret.insert(s);
    };
  ORTHOGONAL_APPLY(process, i, j);
  ret.erase(t_position(i, j));
  return ret;
}

template<int goban_size>
t_stones
Goban<goban_size>::get_liberties(unsigned int i, unsigned int j)
{
  return get_liberties(i, j, board[i][j].color);
}

template<int goban_size>
void
Goban<goban_size>::add_strong_links(unsigned int i, unsigned int j)
{
  /*auto process = [&](int i, int j,
                     int ii, int jj,
                     int imin, int jmin,
                     int imax, int jmax)
    {
  if (i > imin && ii < imax
      && j > jmin && jj < jmax
      && board[ii][j].color == Empty
      && board[i][jj].color == Empty
      && board[ii][jj].color == board[i][j].color)
    strong_links.insert(t_strong_link(t_position(ii, j), t_position(i,jj)));
    };
*/

  /* NW */
  if (i > 0 && j > 0
      && board[i-1][j].color == Empty
      && board[i][j-1].color == Empty
      && board[i-1][j-1].color == board[i][j].color)
    strong_links.insert(t_strong_link(t_position(i-1, j), t_position(i,j-1)));
  /* NE */
  if (i < goban_size-1 && j > 0
      && board[i+1][j].color == Empty
      && board[i][j-1].color == Empty
      && board[i+1][j-1].color == board[i][j].color)
    strong_links.insert(t_strong_link(t_position(i+1, j), t_position(i,j-1)));
  /* SW */
  if (i > 0 && j < goban_size-1
      && board[i-1][j].color == Empty
      && board[i][j+1].color == Empty
      && board[i-1][j+1].color == board[i][j].color)
    strong_links.insert(t_strong_link(t_position(i-1, j), t_position(i,j+1)));
  /* SE */
  if (i < goban_size-1 && j < goban_size-1
      && board[i+1][j].color == Empty
      && board[i][j+1].color == Empty
      && board[i+1][j+1].color == board[i][j].color)
    strong_links.insert(t_strong_link(t_position(i+1, j), t_position(i,j+1)));
}

template<int goban_size>
t_stones
Goban<goban_size>::list_neighbors(unsigned int i, unsigned int j)
{
  assert(board[i][j].color != Empty);

  t_stones ret;

  auto process = [&](unsigned int i, unsigned int j)
    {
      if (board[i][j].color != Empty)
        ret.insert(t_position(i, j));
    };
  ORTHOGONAL_APPLY(process, i, j);
  for (auto s : ret)
    assert(cell(s).color != Empty);

  return ret;
}

template<int goban_size>
void
Goban<goban_size>::remove_stones(t_group* stones)
{
  for (auto stone : stones->stones)
    {
      for (auto neighbor : list_neighbors(stone.first, stone.second))
        this->cell(neighbor).get_group()->liberties.insert(stone);
      auto& cell = this->cell(stone);
      (cell.color == Black ? black_groups : white_groups).remove(stones);
      cell.color = Empty;
    }
}

template<int goban_size>
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
      ;//std::cerr << "duplicate move " << m.first.first << ", " << m.first.second << std::endl;

  potential_moves.clear();
  for (auto m : moves)
    {
      double oldlib = m.second->liberties.size();
      double newlib = (double)get_liberties(m.first.first, m.first.second, player).size();
      double newlib2 = (double)get_liberties(m.first.first, m.first.second, otherplayer).size();

      double w =  2*LIBERTY_REM * (newlib2 / oldlib);
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
  dump_moves();

  t_weighed_stone best_move(t_position(5, 5), -10000);
  for (auto m : potential_moves)
    if (m.second > best_move.second)
      best_move = m;
  //play(best_move.first.first, best_move.first.second, player);
  //potential_moves.erase(best_move);
  std::cerr << "best move: " << best_move.first.first << ", " << best_move.first.second << std::endl;
  return best_move.first;
}

template<int goban_size>
void
Goban<goban_size>::play(unsigned short int i, unsigned short int j, t_color c)
{
  assert(board[i][j].color == Empty);
  assert(c != Empty);

  auto& colorgroup = (c == Black ? black_groups : white_groups);
  board[i][j] = *new Cell(i, j, c);
  auto& new_stone = this->cell(t_position(i, j));
  new_stone.get_group()->liberties = get_liberties(i, j);
  colorgroup.push_front(new_stone.get_group());

  for (auto nei : list_neighbors(i, j))
    {
      auto &neighbor = this->cell(nei);
      auto neighbor_group = neighbor.get_group();
      if (neighbor.color == Empty) //already removed
        continue;
      else if (neighbor.color != c)
        {
          neighbor_group->liberties.erase(t_position(i, j));
          if (neighbor_group->liberties.size() == 0)
            remove_stones(neighbor_group);
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
}

template<int goban_size>
void
Goban<goban_size>::determine_hoshis()
{
  auto insert = [&](unsigned short int i, unsigned short int j)
    {
      hoshis.insert(*new t_position(i, j));
    };

  if (goban_size == 13)
    {
      insert(2, 2);
      insert(2, 6);
      insert(2, 10);
      insert(6, 2);
      insert(6, 6);
      insert(6, 10);
      insert(10, 2);
      insert(10, 6);
      insert(10, 10);
    }
  else if (goban_size == 19)
    {
      insert(3, 3);
      insert(3, 9);
      insert(3, 15);
      insert(9, 3);
      insert(9, 9);
      insert(9, 15);
      insert(15, 3);
      insert(15, 9);
      insert(15, 15);
    }
  else
    std::cerr << "warning: goban has irregular size (" << goban_size
      << "), can't draw proper hoshi." << std::endl;
}

template<int goban_size>
void
Goban<goban_size>::dump()
{
  for (auto i = 0; i < goban_size; ++i)
    {
      for (auto j = 0; j < goban_size; ++j)
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

template<int goban_size>
void
Goban<goban_size>::dump_groups()
{
  std::cout << "Begin black groups dump:" << std::endl;
  dump_group(black_groups);
  std::cout << "Begin white groups dump:" << std::endl;
  dump_group(white_groups);
  std::cout << "End groups dump." << std::endl;
}

template<int goban_size>
void
Goban<goban_size>::dump_group(t_groups groups)
{
  for (auto group : groups)
    {
      std::cout << " - " << group->liberties.size() << "L (" << group << ") ";
      for (auto cell : group->stones)
        std::cout << "(" << cell.first << ", " << cell.second
          << ", " << this->cell(cell).get_group() << ") ";
      std::cout << std::endl;
      std::cout << "   lib: ";
      for (auto cell : group->liberties)
        std::cout << "(" << cell.first << ", " << cell.second<< ") ";
      std::cout << std::endl;
    }
}

template<int goban_size>
void
Goban<goban_size>::dump_links()
{
  std::cout << "Begin strong links dump:" << std::endl;
  for (auto link : strong_links)
    std::cout << " - (" << link.first.first << ", " << link.first.second << ") ("
      << link.second.first << ", " << link.second.second << ")" << std::endl;
  std::cout << "End strong links dump." << std::endl;
}

template<int goban_size>
void
Goban<goban_size>::dump_moves()
{
  std::cerr << "Begin moves dump:" << std::endl;
  for (auto m : potential_moves)
    std::cerr << " - (" << m.first.first << ", " << m.first.second << ") "
      << m.second << std::endl;
  std::cerr << "End moves dump." << std::endl;

}

template<int goban_size>
void
Goban<goban_size>::reset()
{
  strong_links.clear();
  white_groups.clear();
  black_groups.clear();
  potential_moves.clear();

  for (auto i = 0; i < goban_size; ++i)
    for (auto j = 0; j < goban_size; ++j)
      {
        board[i][j].color = Empty;
        board[i][j].set_group(nullptr);
      }
}
