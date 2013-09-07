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
  char row = (char) pos.first + 'A';
  if (row >= 'I')
    ++row;
  os << row << pos.second + 1;
  return os;
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
Goban<goban_size>::genmove(t_color player)
{
  auto move = genmove_opening(player);
  if (move != t_position(PASS, PASS))
    return move;
  move = genmove_liberty(player);
  return move;
}

template<unsigned short goban_size>
t_position
Goban<goban_size>::genmove_opening(t_color player)
{
  for (auto candidate : starting_stones)
    {
      starting_stones.erase(candidate);
      if (this->cell(candidate).color == Empty)
        return candidate;
      else if (this->cell(candidate).color == player) // handicap placed
        continue;
      else
        {
          auto alternatives = get_liberties(candidate);
          if (!alternatives.empty())
            {
              srand((unsigned int) time(0));
              auto it = alternatives.begin();
              std::advance(it, (unsigned long) rand() % alternatives.size());
              return *it;
            }
        }
    }
  return t_position(PASS, PASS);
}

template<unsigned short goban_size>
t_position
Goban<goban_size>::genmove_liberty(t_color player)
{
  std::set<t_position> moves;

  auto select_legal_moves = [&](t_groups groups)
    {
      for (auto g : groups)
        for (auto candidate : g->liberties)
          {
            auto miai = std::find_if(strong_links.begin(),
                                     strong_links.end(),
                                     [=](t_strong_link link) -> bool {
                                     return ((link.color == player)
                                             && (link.first == candidate
                                                 || link.second == candidate)); });
            if (miai != strong_links.end() )
              {
                std::cerr << candidate << ": disgarding miai " << (*miai).first << " "
                  << (*miai).second << std::endl;
                continue;
              }

            // If the move is not a blatant suicide, consider it.
            // Else, look for nearby atari.
            if (0 < get_liberties(candidate.first, candidate.second,
                                  player).size())
              moves.insert(candidate);
            else
              for (auto n : get_neighbors(candidate))
                if (this->cell(n).color != player
                    && this->cell(n).get_group()->liberties.size() == 1)
                  moves.insert(candidate);
          }
    };
  select_legal_moves(white_groups);
  select_legal_moves(black_groups);
  moves.erase(ko);
  std::cerr << "moves: ";
  for (auto a: moves)
    std::cerr << a << " ";
  std::cerr << std::endl;

  potential_moves.clear();

  for (auto m : moves)
    {
      std::set<t_group*> neighbor_groups;
      std::list<t_motiv> scoring;

      for (auto n : get_neighbors(m))
        {
          auto cell = this->cell(n);
          auto group = cell.get_group();
          auto lib = group->liberties.size();

          // A move affects groups, not single stones: if it is in contact with
          // two stones of a group, it's effect is not doubled.
          if (false == neighbor_groups.insert(group).second)
            continue;

          auto neli = get_liberties(m.first, m.second, player).size();
          if (cell.color == player)
            {
              if (lib == 1 && neli > lib)
                scoring.push_front(Atari_esc);
              if (lib < 3 && neli >= 3)
                scoring.push_front(Lib_suf);
              if (lib < neli)
                scoring.push_front(Lib_inc);
            }
          if (cell.color != player)
            {
              for (auto v : get_neighbors(n))
                if (this->cell(v).get_group()->liberties.size() == 1)
                  scoring.push_front(Atari_esc);
              if (lib == 1)
                scoring.push_front(Kill);
              if (lib == 2)
                scoring.push_front(Atari_set);
              else
                scoring.push_front(Lib_dec);
            }
        }
      if (1 == get_liberties(m.first, m.second, player).size())
        scoring.push_front(Atari_self);
      else if (0 == get_liberties(m.first, m.second, Empty).size())
        scoring.push_front(Fill);

      std::cerr << m << " scoring:";
      //if (scoring.find(Atari_esc))
      for (auto x : scoring)
        if (x == Kill)
          {
            scoring.remove(Atari_self);
            scoring.remove(Fill);
          }
      for (auto x : scoring)
        if (x == Atari_esc)
          scoring.remove(Atari_set);
      double weight = 0;
      for (auto s : scoring)
        {
          std::cerr << " " << scorer[s].str;
          weight += scorer[s].value;
        }
      std::cerr << std::endl;
      //std::cerr << "=> " << weight << std::endl;
      potential_moves.push_back(t_weighed_stone(m, weight));
    }
  std::sort(potential_moves.begin(),
            potential_moves.end(),
            [](const t_weighed_stone& a,
               const t_weighed_stone& b) -> bool {
              return a.second < b.second;
              });


  t_stones best_moves;
  double best_score = 0.0;
  for (auto m : potential_moves)
    {
      if (m.second - best_score > 0.001)
        best_moves.insert(m.first);
      else if (m.second > best_score)
        {
          best_moves.clear();
          best_moves.insert(m.first);
          best_score = m.second;
        }
    }

  /* 'pass' should be something like (0, 0) or (-1, -1), alas our
  ** representation is 0-based and unsigned.  */
  if (best_moves.empty())
    return t_position(PASS, PASS);

  srand((unsigned int) time(0));
  auto it = best_moves.begin();
  std::advance(it, (unsigned long) rand() % best_moves.size());

  auto best_move = *it;
  std::cerr << "best move: " << best_move
    << " (" << best_score << ")" << std::endl;

  if (best_score <= 0.8)
    for (auto alt : get_liberties(best_move))
      {
        if (moves.count(alt))
          std::cerr << "alt " << alt << " was found" << std::endl;
        else
          {
            std::cerr << "playing alt " << alt << std::endl;
            return alt;
          }
      }
  return best_move;
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
  determine_hoshis();

  for (unsigned short i = 0; i < goban_size; ++i)
    for (unsigned short j = 0; j < goban_size; ++j)
      {
        board[i][j].color = Empty;
        board[i][j].set_group(nullptr);
      }
  ko = t_position(PASS, PASS);
}
