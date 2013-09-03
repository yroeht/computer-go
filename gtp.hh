#ifndef GTP_HH
# define GTP_HH

#include <string>
#include <functional>
#include <set>
#include <goban.hh>

#ifndef __NAME
# error A value must be defined for __NAME (try -D).
#endif

// cheap double expansion quoting trick...
#define STR_(x) #x
#define STR(x) STR_(x)
#define __NAME_STR STR(__NAME)

#ifndef __GOBAN_SIZE
# error A value must be defined for __GOBAN_SIZE (try -D).
#endif

static Goban<__GOBAN_SIZE> goban;

void gtp_boardsize();
void gtp_clear_board();
void gtp_genmove();
void gtp_list_commands();
void gtp_name();
void gtp_play();
void gtp_protocol_version();
void gtp_quit() __attribute__((noreturn));
void gtp_showboard();
void gtp_version();

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
    {"showboard",           gtp_showboard},
};

#define COMMANDS 10

void gtp_loop() __attribute__((noreturn));

#endif /* GTP_HH */
