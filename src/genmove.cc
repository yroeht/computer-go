#include <algorithm>

#include <genmove.hh>
#include <golib.hh>
#include <iostream>

t_position
Genmove::genmove_opening(t_color player,
                         t_stones& starting_stones)
{
  for (auto candidate : starting_stones)
    {
      starting_stones.erase(candidate);
      if (this->cell(candidate).color == Empty)
        return candidate;
      else if (this->cell(candidate).color == player) // handicap placed
        continue;
      else
        {
          auto alternatives = get_liberties(candidate, board);
          if (!alternatives.empty())
            {
              srand((unsigned int) time(0));
              auto it = alternatives.begin();
              std::advance(it, (unsigned long) rand() % alternatives.size());
              return *it;
            }
        }
    }
  return t_position(PASS, PASS);
}

t_position
Genmove::genmove_liberty(t_color player,
                         t_strong_links strong_links)
{
  t_weighed_stones  potential_moves;
  std::set<t_position> moves;

  auto select_legal_moves = [&](t_groups groups)
    {
      for (auto g : groups)
        for (auto candidate : g->liberties)
          {
            auto miai = std::find_if(strong_links.begin(),
                                     strong_links.end(),
                                     [=](t_strong_link link) -> bool {
                                     return ((link.color == player)
                                             && (link.first == candidate
                                                 || link.second == candidate)); });
            if (miai != strong_links.end() )
              {
                std::cerr << candidate << ": disgarding miai " << (*miai).first << " "
                  << (*miai).second << std::endl;
                continue;
              }

            // If the move is not a blatant suicide, consider it.
            // Else, look for nearby atari.
            if (0 < get_liberties(candidate.first, candidate.second,
                                  player, board).size())
              moves.insert(candidate);
            else
              for (auto n : get_neighbors(candidate, board))
                if (this->cell(n).color != player
                    && this->cell(n).get_group()->liberties.size() == 1)
                  moves.insert(candidate);
          }
    };
  select_legal_moves(white_groups);
  select_legal_moves(black_groups);
  moves.erase(ko);
  std::cerr << "moves: ";
  for (auto a: moves)
    std::cerr << a << " ";
  std::cerr << std::endl;

  potential_moves.clear();

  for (auto m : moves)
    {
      std::set<t_group*> neighbor_groups;
      std::list<t_motiv> scoring;

      for (auto n : get_neighbors(m, board))
        {
          auto cell = this->cell(n);
          auto group = cell.get_group();
          auto lib = group->liberties.size();
          auto neli = get_liberties(m.first, m.second, player, board).size();

          // A move affects groups, not single stones: if it is in contact with
          // two stones of a group, it's effect is not doubled.
          if (false == neighbor_groups.insert(group).second)
            continue;

          if (cell.color == player)
            {
              if (lib == 1 && neli > lib)
                scoring.push_front(Atari_esc);
              if (lib < 3 && neli >= 3)
                scoring.push_front(Lib_suf);
              if (lib < neli)
                scoring.push_front(Lib_inc);
            }
          if (cell.color != player)
            {
              for (auto v : get_neighbors(n, board))
                if (this->cell(v).get_group()->liberties.size() == 1)
                  scoring.push_front(Atari_esc);
              if (lib == 1)
                scoring.push_front(Kill);
              if (lib == 2)
                scoring.push_front(Atari_set);
              else
                scoring.push_front(Lib_dec);
            }
        }
      if (1 == get_liberties(m.first, m.second, player, board).size())
        scoring.push_front(Atari_self);
      else if (0 == get_liberties(m.first, m.second, Empty, board).size())
        scoring.push_front(Fill);

      std::cerr << m << " scoring:";
      //if (scoring.find(Atari_esc))
      for (auto x : scoring)
        if (x == Kill)
          {
            scoring.remove(Atari_self);
            scoring.remove(Fill);
          }
      for (auto x : scoring)
        if (x == Atari_esc)
          scoring.remove(Atari_set);
      double weight = 0;
      for (auto s : scoring)
        {
          std::cerr << " " << scorer[s].str;
          weight += scorer[s].value;
        }
      std::cerr << "=> " << weight;
      std::cerr << std::endl;
      potential_moves.push_back(t_weighed_stone(m, weight));
      std::cerr << "[" << potential_moves.back().second << "]" << std::endl;
    }
  std::sort(potential_moves.begin(),
            potential_moves.end(),
            [](const t_weighed_stone& a,
               const t_weighed_stone& b) -> bool {
              return a.second < b.second;
              });


  std::cerr << "[(((" << potential_moves.back().second << ")))] "<< potential_moves.size() << std::endl;
  t_stones best_moves;
  double best_score = 0.0;
  for (auto m : potential_moves)
    {
      std::cerr << "loop " << m.second << " " << best_score << " "
        << (m.second - best_score  < 0.001) << " " << best_moves.size() << std::endl;
      if (fabs(m.second - best_score) < 0.001)
        best_moves.insert(m.first);
      else if (m.second > best_score > 0.001)
        {
          best_moves.clear();
          best_moves.insert(m.first);
          best_score = m.second;
        }
      else
        std::cerr << "shit: " << m.second << std::endl;
    }

  /* 'pass' should be something like (0, 0) or (-1, -1), alas our
  ** representation is 0-based and unsigned.  */
  if (best_moves.empty())
    return t_position(PASS, PASS);

  srand((unsigned int) time(0));
  auto it = best_moves.begin();
  std::advance(it, (unsigned long) rand() % best_moves.size());

  auto best_move = *it;
  std::cerr << "best move: " << best_move
    << " (" << best_score << ")" << std::endl;

  if (best_score <= 0.8)
    for (auto alt : get_liberties(best_move, board))
      {
        if (moves.count(alt))
          std::cerr << "alt " << alt << " was found" << std::endl;
        else
          {
            std::cerr << "playing alt " << alt << std::endl;
            return alt;
          }
      }
  return best_move;
}

