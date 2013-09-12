#ifndef TYPES_HH
# define TYPES_HH

# include <set>
# include <list>
# include <vector>

# define PASS 999

typedef std::pair<unsigned short,
                  unsigned short>           t_position;

typedef std::set<t_position>                t_stones;

struct                                      t_group
{
  t_stones stones;
  t_stones liberties;
};

typedef std::list<t_group*>                 t_groups;

typedef enum {
    Empty,
    Black,
    White,
}                                           t_color;

/* remove me asap */
typedef struct
{
  t_position first;
  t_position second;
  t_color    color;
} t_strong_link;

typedef std::list<t_strong_link>            t_strong_links;
/* end-of-remove-asap */

class Cell;

typedef std::vector<std::vector<Cell> >     t_board;

#endif /* TYPES_HH */
