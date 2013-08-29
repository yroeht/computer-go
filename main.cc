#include <goban.hh>
#include <gtp.hh>

template<int n>
void
eye_capture(Goban<n> goban)
{
  goban.play(2, 3, Black);
  goban.play(3, 3, White);
  goban.play(2, 6, Black);
  goban.play(3, 4, White);
  goban.play(2, 4, White);
  goban.play(1, 3, White);

  /* White captures one black stone */

  goban.play(2, 2, White);
  goban.play(1, 2, White);


  goban.play(0, 2, Black);
  goban.play(0, 3, Black);
  goban.play(1, 1, Black);
  goban.play(1, 4, Black);
  goban.play(2, 1, Black);
  
  goban.play(3, 2, Black);
  goban.play(2, 5, Black);
  goban.play(3, 5, Black);
  goban.play(4, 3, Black);
  goban.play(4, 4, Black);
  goban.dump();
  goban.dump_groups();
  goban.play(2, 3, Black);
  goban.dump();
  goban.dump_groups();

  /* Black kills the white false-eye */

  goban.play(0, 1, Black);
  goban.play(0, 4, Black);
  goban.play(3, 1, Black);
  goban.play(4, 2, Black);
  goban.play(2, 4, Black);

  goban.dump();
  goban.dump_groups();

  /* And forms two eyes */

}

template<int n>
void
links(Goban<n> goban)
{
  goban.play(6, 5, Black);
  goban.play(8, 7, Black);
  goban.play(10, 5, Black);
  goban.play(8, 3, Black);

  goban.dump();
  goban.dump_groups();
  goban.dump_links();

  goban.play(7, 5, Black);
  goban.play(8, 4, Black);
  goban.play(8, 6, Black);
  goban.play(9, 5, Black);
  goban.dump();
  goban.dump_groups();
  goban.dump_links();
}

template<int n>
void
atari(Goban<n> goban)
{
  goban.play(7, 5, White);
  goban.play(7, 6, White);
  goban.play(7, 4, Black);
  goban.play(6, 5, Black);
  goban.play(6, 6, Black);
  goban.play(7, 7, Black);
  goban.play(8, 6, Black);
  goban.dump();

  goban.act_on_atari(White);
  goban.dump();
  goban.act_on_atari(Black);
  goban.dump();

  goban.act_on_atari(White);
  goban.dump();
  goban.act_on_atari(Black);
  goban.dump();

  goban.act_on_atari(White);
  goban.dump();
  goban.act_on_atari(Black);
  goban.dump();

  goban.act_on_atari(White);
  goban.dump();
  goban.act_on_atari(Black);
  goban.dump();

  goban.act_on_atari(White);
  goban.dump();
  goban.act_on_atari(Black);
  goban.dump();

  goban.act_on_atari(White);
  goban.dump();
  goban.act_on_atari(Black);
  goban.dump();
}

int main()
{
  goban.determine_hoshis();
  gtp_loop();
  //Goban<19> goban;
  //eye_capture(goban);
  //links(goban);
  atari(goban);
}
