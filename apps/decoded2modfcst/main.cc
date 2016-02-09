#include <iostream>
#include <fstream>
#include <sstream>

#include "decoded2modfcst.hh"

using namespace std;

//
// Prototypes for static functions.
//

static void tidy_and_exit (int sig);

//
// Global variables
//

static decoded2modfcst *Prog;


/*****************************************************************
 * main() - main program.
 */

int main(int argc, char **argv)
{
  // create program object
  
  Prog = new decoded2modfcst(argc, argv);
  
  // run it
  int ret = Prog->run(argv);
  
  // clean up
  tidy_and_exit(ret);
  return (ret);
}

/*****************************************************************
 * tidy_and_exit() - Clean up memory and exit from the program.
 */

static void tidy_and_exit(int sig)
{
  delete(Prog);
  exit(sig);
}
