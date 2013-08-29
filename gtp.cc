#include <gtp.hh>
#include <iostream>

static void
gtp_success(const std::string& msg)
{
  std::cout << "= " << msg << std::endl << std::endl;
}

static void
gtp_failure(const std::string& msg)
{
  std::cout << "? " << msg << std::endl << std::endl;
}

static void
gtp_comment(const std::string& msg)
{
  std::cout << "# " << msg << std::endl;
}

void
gtp_list_commands()
{
  std::string commands_buffer = "";

  for (auto i = 0; i < COMMANDS; ++i)
    commands_buffer += commands[i].name + "\n";
  gtp_success(commands_buffer);
}

void
gtp_name()
{
  gtp_success("Goku");
}

void
gtp_protocol_version()
{
  gtp_success("2");
}

void
gtp_version()
{
  gtp_success("0.1 dev");
}

void
gtp_boardsize()
{
  int new_size;
  std::cin >> new_size;
  if (new_size == 13 || new_size == 19 || new_size == 7)
    gtp_success("");
  else
    gtp_failure("unacceptable size");
}
void
gtp_clear_board()
{
  gtp_success("");
}

void
gtp_quit()
{
  gtp_success("");
  exit(0);
}

void
gtp_play()
{
  char color;
  std::cin >> color;

  std::string move;
  std::cin >> move;

  unsigned int row = move[0] - 'A';
  /* Deal with "I" */
  if (row >= 9)
    row--;

  unsigned int line = move[1] - '0';
  if (move[2] >= '0' && move[2] <= '9')
    line = line * 10 + move[2] - '0';
  line--;

  goban.play(row, line, (color == 'B') ? Black : White);
  gtp_success("");
}

void
gtp_genmove()
{
  char color;
  std::cin >> color;
  t_color player = (color == 'b') ? Black : White;

  t_position move;

  move = goban.act_on_atari(player);
  goban.play(move.first, move.second, player);

  /* Deal with "I" */
  if (move.first >= 8)
    move.first++;

  std::string s;
  s.push_back('A' + move.first);
  s += std::to_string(move.second + 1);
  gtp_success(s);
}

void
gtp_loop()
{
  while (true)
    {
      std::string input;
      std::cin >> input;
      auto i = 0;
      for (; i < COMMANDS; ++i)
        if (input.compare(commands[i].name) == 0)
          {
            commands[i].function();
            break;
          }
      if (i == COMMANDS)
        gtp_failure("Unsupported command<" + input + ">" );
    }
}