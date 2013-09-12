#ifndef CELL_HXX
# define CELL_HXX

inline
Cell::Cell(unsigned short i, unsigned short j, t_color color_)
{
  if (Empty == (this->color = color_))
    this->group = nullptr;
  else
    {
      this->group = new t_group();
      this->group->stones.insert(t_position(i, j));
    }
}

inline
t_group*
Cell::get_group() const
{
  return this->group;
}

inline
void
Cell::set_group(t_group* group_)
{
  this->group = group_;
}

#endif /* CELL_HXX */
