#ifndef GOBAN_HH
# define GOBAN_HH

# include <list>
# include <set>
# include <vector>

# include <ostream>

# include <types.hh>
# include <golib.hh>
# include <genmove.hh>

template<unsigned short goban_size_>
class Goban
{
public:
  t_board           board;

private:
  t_strong_links    strong_links;
  t_groups          white_groups;
  t_groups          black_groups;
  t_stones          hoshis;
  t_position        ko;
  t_stones          starting_stones;

public:
  Goban();
  Cell& cell(t_position);
  void determine_hoshis();
  void dump();
  void dump_groups();
  void dump_links();
  bool play(unsigned short, unsigned short, t_color);
  t_position genmove(t_color player);
  t_position genmove_liberty(t_color player);
  t_position genmove_opening(t_color player);
  void reset();

private:
  void      remove_stones(t_group*);
  void      dump_group(t_groups);
  void      add_strong_links(unsigned short i, unsigned short j);
};

# include <goban.hxx>

#endif /* GOBAN_HH */

