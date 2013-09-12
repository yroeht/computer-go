inline
Genmove::Genmove(const t_board* board_,
                 const t_position ko_,
                 const t_groups black_groups_,
                 const t_groups white_groups_)
  : board(board_)
  , ko(ko_)
  , black_groups(black_groups_)
  , white_groups(white_groups_)

{
}

inline
const Cell&
Genmove::cell(t_position stone)
{
  return (*board)[stone.first][stone.second];
}
