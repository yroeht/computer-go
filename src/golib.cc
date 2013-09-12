#include <golib.hh>

#define goban_size (*board).size()

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

std::ostream& operator<<(std::ostream& os, const t_position& pos)
{
  char row = (char) pos.first + 'A';
  if (row >= 'I')
    ++row;
  os << row << pos.second + 1;
  return os;
}

t_stones
get_liberties(unsigned short i, unsigned short j, t_color color, const t_board* board)
{
  t_stones ret;
  auto process = [&](unsigned short x, unsigned short y)
    {
      auto& cell = (*board)[x][y];
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

t_stones
get_liberties(unsigned short i, unsigned short j, const t_board* board)
{
  return get_liberties(i, j, (*board)[i][j].color, board);
}

t_stones
get_liberties(const t_position pos, const t_board* board)
{
  return get_liberties(pos.first, pos.second, board);
}

t_stones
get_neighbors(unsigned short i, unsigned short j, const t_board* board)
{
  t_stones ret;

  auto process = [&](unsigned short x, unsigned short y)
    {
      if ((*board)[x][y].color != Empty)
        ret.insert(t_position(x, y));
    };
  ORTHOGONAL_APPLY(process, i, j);

  return ret;
}

t_stones
get_neighbors(t_position pos, const t_board* board)
{
  return get_neighbors(pos.first, pos.second, board);
}


