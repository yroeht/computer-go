#include <iostream>
#include <set>
#include <algorithm>
#include <assert.h>

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
void
Goban<goban_size>::add_strong_links(unsigned short i, unsigned short j)
{
  auto process = [&](unsigned short ii, unsigned short jj)
    {
      if ( board[ii][j].color == Empty
          && board[i][jj].color == Empty
          && board[ii][jj].color == board[i][j].color)
        {
          t_strong_link link;
          link.first = t_position(ii, j);
          link.second = t_position(i, jj);
          link.color = board[i][j].color;
          strong_links.push_front(link);
        }
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
void
Goban<goban_size>::remove_stones(t_group* stones)
{
  for (auto stone : stones->stones)
    {
      for (auto neighbor : get_neighbors(stone.first, stone.second, &board))
        this->cell(neighbor).get_group()->liberties.insert(stone);
      auto& cell = this->cell(stone);
      (cell.color == Black ? black_groups : white_groups).remove(stones);
      cell.color = Empty;
    }
}

template<unsigned short goban_size>
t_position
Goban<goban_size>::genmove(t_color player)
{
  Genmove generator = Genmove(&board, ko, black_groups, white_groups);
  auto move = generator.genmove_opening(player, starting_stones);
  if (move != t_position(PASS, PASS))
    return move;
  move = generator.genmove_liberty(player, strong_links);
  return move;
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
  new_stone.get_group()->liberties = get_liberties(i, j, &board);
  colorgroup.push_front(new_stone.get_group());

  for (auto nei : get_neighbors(i, j, &board))
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
          for (auto l : neighbor_group->liberties)
            new_stone.get_group()->liberties.insert(l);
          new_stone.get_group()->liberties.erase(t_position(i, j));
          colorgroup.remove(neighbor_group);
        }
    }
  add_strong_links(i, j);
  std::remove_if(strong_links.begin(),
                 strong_links.end(),
                 [i,j](t_strong_link link ) -> bool {
                 return (link.first == t_position(i, j)
                         || link.second == t_position(i, j));
                 });
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
  std::cerr << "  A B C D E F G H J K L M N" << std::endl;
  for (unsigned short j = goban_size - 1; j < PASS; --j)
    {
      std::cerr << (j+1) % 10 << " ";
      for (unsigned short i = 0; i < goban_size; ++i)
        {
          auto cell = board[i][j].color;
          if (Black == cell)
            std::cerr << "X";
          else if (White == cell)
            std::cerr << "O";
          else if (Empty == cell
                   && hoshis.end() != std::find(hoshis.begin(),
                                                hoshis.end(),
                                                t_position(i, j)))
            std::cerr  << "*";
          else
            std::cerr << ".";
          std::cerr << " ";
        }
      std::cerr << (j+1) % 10;
      std::cerr << std::endl;
    }
  std::cerr << "  A B C D E F G H J K L M N" << std::endl;
  std::cerr << std::endl;
}

template<unsigned short goban_size>
void
Goban<goban_size>::dump_groups()
{
  std::cerr << "Begin black groups dump:" << std::endl;
  dump_group(black_groups);
  std::cerr << "Begin white groups dump:" << std::endl;
  dump_group(white_groups);
  std::cerr << "End groups dump." << std::endl;
}

template<unsigned short goban_size>
void
Goban<goban_size>::dump_group(t_groups groups)
{
  for (auto group : groups)
    {
      for (auto cell : group->stones)
        std::cerr << cell << " " << this->cell(cell).get_group() << " ";
      std::cerr << std::endl;
      std::cerr << "   lib: ";
      for (auto cell : group->liberties)
        std::cerr << cell << " ";
      std::cerr << std::endl;
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
Goban<goban_size>::reset()
{
  strong_links.clear();
  white_groups.clear();
  black_groups.clear();
  determine_hoshis();

  for (unsigned short i = 0; i < goban_size; ++i)
    for (unsigned short j = 0; j < goban_size; ++j)
      {
        board[i][j].color = Empty;
        board[i][j].set_group(nullptr);
      }
  ko = t_position(PASS, PASS);
}
