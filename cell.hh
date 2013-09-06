#ifndef CELL_HH
# define CELL_HH

typedef std::pair<unsigned short,
                  unsigned short>           t_position;

typedef std::set<t_position>                t_stones;

struct t_group
{
  t_stones stones;
  t_stones liberties;
};

typedef enum {
    Empty,
    Black,
    White,
} t_color;

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

#include <cell.hxx>

#endif /* CELL_HH */
