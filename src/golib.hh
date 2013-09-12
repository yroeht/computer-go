#ifndef GOLIB_HH
# define GOLIB_HH

# include <ostream>
# include <types.hh>
# include <cell.hh>

std::ostream& operator<<(std::ostream& os, const t_position& pos);

t_stones
get_liberties(unsigned short i, unsigned short j, t_color color,
              const t_board* board);

t_stones
get_liberties(unsigned short i, unsigned short j, const t_board* board);

t_stones
get_liberties(const t_position pos, const t_board* board);

t_stones
get_neighbors(unsigned short i, unsigned short j, const t_board* board);

t_stones
get_neighbors(t_position pos, const t_board* board);

#endif /* GOLIB_HH */
