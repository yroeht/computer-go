#ifndef GOBAN_HH
# define GOBAN_HH

# include <list>
# include <set>
# include <vector>
# include <ostream>


typedef enum {
    Empty,
    Black,
    White,
} t_color;

typedef std::pair<unsigned short,
                  unsigned short>           t_position;

typedef std::set<t_position>                t_stones;

typedef std::pair<t_position, double>       t_weighed_stone;
typedef std::vector<t_weighed_stone>        t_weighed_stones;

typedef std::pair<t_position, t_position>   t_strong_link;
typedef std::set<t_strong_link>             t_strong_links;

class t_group
{
public:
  t_stones stones;
  t_stones liberties;
};

typedef std::list<t_group*> t_groups;

class Cell
{
public:
  t_color color;

private:
  t_group *group;

public:
  Cell(unsigned short, unsigned short, t_color color);
  t_group *get_group();
  void  set_group(t_group*);
};

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

public:
  Goban();
  Cell& cell(t_position);
  void determine_hoshis();
  void dump();
  void dump_moves();
  void dump_groups();
  void dump_links();
  void play(unsigned short, unsigned short, t_color);
  t_position act_on_atari(t_color player);
  void reset();

private:
  void      remove_stones(t_group*);
  void      dump_group(t_groups);
  void      add_strong_links(unsigned short i, unsigned short j);
  t_stones  list_neighbors(unsigned short i, unsigned short j);
  t_stones  get_liberties(unsigned short i, unsigned short j);
  t_stones  get_liberties(unsigned short i, unsigned short j, t_color c);
};

std::ostream& operator<<(std::ostream& os, const t_position& pos);

# include <goban.hxx>

#endif /* GOBAN_HH */

