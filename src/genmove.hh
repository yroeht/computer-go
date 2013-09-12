#ifndef GENMOVE_HH
# define GENMOVE_HH

# include <types.hh>
# include <string>

typedef enum {
    Atari_esc,
    Atari_set,
    Lib_inc,
    Lib_suf,
    Lib_dec,
    Fill,
    Kill,
    Atari_self
} t_motiv;

typedef struct {
    t_motiv     motiv;
    double      value;
    std::string str;
} t_score;

const t_score scorer[] =
{
    { Atari_esc,    6.0,    "Atari_esc" },
    { Atari_set,    3.0,    "Atari_set" },
    { Lib_inc,      0.5,    "Lib_inc" },
    { Lib_suf,      2.0,    "Lib_suf" },
    { Lib_dec,      0.3,    "Lib_dec" },
    { Fill,         -1.5,   "Fill" },
    { Kill,         3.0,    "Kill" },
    { Atari_self,   -20,    "Atari_self" }
};

typedef std::pair<t_position, double>       t_weighed_stone;
typedef std::vector<t_weighed_stone>        t_weighed_stones;

class Genmove
{
public:
  Genmove(const t_board*,
          const t_position ko_,
          const t_groups black_groups_,
          const t_groups white_groups_);
  t_position genmove_opening(t_color,
                             t_stones& starting_stones);
  t_position genmove_liberty(t_color,
                             t_strong_links strong_links);
private:
  const Cell& cell(t_position stone);

private:
  const t_board*        board;
  const t_position      ko;
  const t_groups        black_groups;
  const t_groups        white_groups;
};

# include <genmove.hxx>

#endif /* GENMOVE_HH */
