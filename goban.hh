#ifndef GOBAN_HH
# define GOBAN_HH

# include <list>
# include <set>
# include <vector>
# include <ostream>
# include <cell.hh>

# define PASS 999

typedef std::pair<t_position, double>       t_weighed_stone;
typedef std::vector<t_weighed_stone>        t_weighed_stones;

typedef struct
{
  t_position first;
  t_position second;
  t_color    color;
} t_strong_link;

typedef std::list<t_strong_link>            t_strong_links;

typedef std::list<t_group*> t_groups;

template<unsigned short goban_size_>
class Goban
{
public:
  std::vector<std::vector<Cell> > board;

private:
  t_strong_links    strong_links;
  t_groups          white_groups;
  t_groups          black_groups;
  t_stones          hoshis;
  t_weighed_stones  potential_moves;
  t_position        ko;
  t_stones          starting_stones;

public:
  Goban();
  Cell& cell(t_position);
  void determine_hoshis();
  void dump();
  void dump_moves();
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
  t_stones  get_neighbors(const t_position pos);
  t_stones  get_neighbors(unsigned short i, unsigned short j);
  t_stones  get_liberties(const t_position pos);
  t_stones  get_liberties(unsigned short i, unsigned short j);
  t_stones  get_liberties(unsigned short i, unsigned short j, t_color c);
};

std::ostream& operator<<(std::ostream& os, const t_position& pos);

# include <goban.hxx>

#endif /* GOBAN_HH */

