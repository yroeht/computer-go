#ifndef GTP_HH
# define GTP_HH

#include <string>
#include <functional>
#include <set>
#include <goban.hh>

static Goban<13> goban;

void gtp_name();
void gtp_protocol_version();
void gtp_version();
void gtp_list_commands();
void gtp_boardsize();
void gtp_clear_board();
void gtp_quit();
void gtp_play();
void gtp_genmove();

typedef void (*gtp_fn_ptr)();

typedef struct {
    const std::string name;
    const gtp_fn_ptr  function;
} t_command;

static const t_command commands[] =
{
    {"name",                gtp_name},
    {"protocol_version",    gtp_protocol_version},
    {"version",             gtp_version},
    {"list_commands",       gtp_list_commands},
    {"boardsize",           gtp_boardsize},
    {"clear_board",         gtp_clear_board},
    {"quit",                gtp_quit},
    {"play",                gtp_play},
    {"genmove",             gtp_genmove},
};

#define COMMANDS 9

void gtp_loop();

#endif /* GTP_HH */
