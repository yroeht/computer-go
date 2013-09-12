#ifndef CELL_HH
# define CELL_HH

# include <types.hh>

class Cell
{
public:
  t_color color;

private:
  t_group *group;

public:
  Cell(unsigned short, unsigned short, t_color color);
  t_group *get_group() const;
  void  set_group(t_group*);
};

# include <cell.hxx>

#endif /* CELL_HH */
