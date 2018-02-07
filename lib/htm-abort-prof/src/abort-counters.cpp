#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define ENV_MAP_FN "MAP_FN"

using namespace std;

string map_fn = "htm-abort.map";
string counter_fn = "htm-abort.ctr";

void parse_args(int argc, char **argv) {
  int c;

  while((c = getopt(argc, argv, "m:c:h")) != -1) {
    switch(c) {
    case 'h':
      cout << argv[0] << " -- parse & display counter data\n\n"
           << "Options:\n"
           << "  -m name : map filename printed by compiler "
           << "(default: '" << map_fn << "')\n"
           << "  -c name : counter filename generated by application "
           << "(default: '" << counter_fn << "')\n";
      exit(0);
      break;
    case 'm': map_fn = optarg; break;
    case 'c': counter_fn = optarg; break;
    default:
      cerr << "WARNING: Unknown argument '" << c << "'\n";
      break;
    }
  }
}

size_t parse_map_file(list<string> &basic_blocks) {
  ifstream fp(map_fn.c_str());
  string bb;
  size_t longest = 0;

  if(!fp.is_open() || !fp.good()) {
    cerr << "WARNING: Could not open map file '" << map_fn << "'!\n";
    exit(1);
  }

  basic_blocks.clear();
  fp >> bb;
  while(!fp.eof()) {
    basic_blocks.push_back(bb);
    if(bb.size() > longest) longest = bb.size();
    fp >> bb;
  }
  if(bb != "") basic_blocks.push_back(bb);

  fp.close();
  return longest;
}

void parse_counter_file(list<uint64_t> &counters) {
  ifstream fp(counter_fn.c_str());
  uint64_t counter;

  if(!fp.is_open() || !fp.good()) {
    cerr << "WARNING: Could not open counter file '" << counter_fn << "'!\n";
    exit(2);
  }

  counters.clear();
  fp >> counter;
  while(!fp.eof()) {
    counters.push_back(counter);
    counter = UINT64_MAX;
    fp >> counter;
  }
  if(counter != UINT64_MAX) counters.push_back(counter);

  fp.close();
}

int main(int argc, char **argv) {
  list<string> basic_blocks;
  list<uint64_t> counters;
  size_t longest;

  parse_args(argc, argv);
  longest = parse_map_file(basic_blocks);
  parse_counter_file(counters);

  list<string>::const_iterator bbit = basic_blocks.begin();
  list<uint64_t>::const_iterator ctrit = counters.begin();
  cout << left << setw(longest + 3) << "Basic block:" << "Counter:" << endl;
  for(; bbit != basic_blocks.end(); bbit++, ctrit++)
    cout << left << setw(longest + 3) << *bbit << *ctrit << "\n";

  return 0;
}
