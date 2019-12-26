#include <fstream>
#include <iostream>
#include "board.h"

using std::ifstream;
using std::ofstream;
using std::endl;
using std::cerr;

using state = Board::state;

int main(int argc, char *argv[])
{
  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open() || !output.is_open())
  {
    cerr << "open files failes." << endl;
    return 0;
  }

  state init_state(9);

  for (int i = 0; i < 9; ++i)
  {
    if (!(input >> init_state[i]))
    {
      cerr << "wrong data." << endl;
      return 0;
    }
  }

  try
  {
    Board board(init_state);
    board.search();
    board.print(output);
  }
  catch(const std::bad_alloc &e)
  {
    cerr << e.what() << endl;
  }
  

  return 0;
}