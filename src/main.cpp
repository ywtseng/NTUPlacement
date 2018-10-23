#include "legalizer/legalizer.hpp"
#include "legalizer/detailed.hpp"
#include "parser/parser.hpp"

#ifdef OMP
#include <omp.h>
#endif

#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  // Parse command line arguments.

  po::options_description options("Options");

  // clang-format off
  options.add_options()
    ("help,h", "Print help messages")
    ("tech_lef", po::value<string>()->value_name("FILE")->required(), "Technology Specification")
    ("cell_lef", po::value<string>()->value_name("FILE")->required(), "Cell Library")
    ("input_def", po::value<string>()->value_name("FILE")->required(), "Global placement")
    ("placement_constraints", po::value<string>()->value_name("FILE")->required(), "Placement constraints")
    ("output_def", po::value<string>()->value_name("FILE")->required(), "Legalization result")
    ("cpu", po::value<int>()->value_name("NUM")->default_value(1), "# of CPUs")
    ("pgp", po::value<string>()->value_name("FILE"), "Plot global placement")
    ("plg", po::value<string>()->value_name("FILE"), "Plot legalization result")
    ;
  // clang-format on

  po::positional_options_description positional_options;
  positional_options.add("tech_lef", 1);
  positional_options.add("cell_lef", 1);
  positional_options.add("input_def", 1);
  positional_options.add("placement_constraints", 1);
  positional_options.add("output_def", 1);

  po::variables_map arguments;
  po::store(po::command_line_parser(argc, argv)
                .options(options)
                .positional(positional_options)
                .style(po::command_line_style::unix_style |
                       po::command_line_style::allow_long_disguise)
                .run(),
            arguments);

  if (argc == 1 || arguments.count("help")) {
    cout << "Usage:" << endl;
    cout << "  " << argv[0] << " [options] "
         << "<tech_lef> <cell_lef> <input_def> "
            "<placement_constraints> <output_def>"
         << endl;
    cout << options;
    return 0;
  }

  // Check if there is any error such as missing option.
  po::notify(arguments);

  const string tech_lef_name = arguments["tech_lef"].as<string>();
  const string cell_lef_name = arguments["cell_lef"].as<string>();
  const string global_placed_def_name = arguments["input_def"].as<string>();
  const string placement_constraints_name =
      arguments["placement_constraints"].as<string>();
  const string legalized_def_name = arguments["output_def"].as<string>();
  const int num_cpus = arguments["cpu"].as<int>();

#ifdef OMP
  omp_set_num_threads(num_cpus);
#endif

  // Parse all files into database.
  Database database;
  Parser parser(database, tech_lef_name, cell_lef_name, global_placed_def_name,
                placement_constraints_name);

  /* database.Print(); */
  database.Summary();

  
  //only check whethether legal	
  Legalizer legalizer(database);
  legalizer.Legalize();
  
  
  //Detailed Placement
  Detailed detailed(database);
  detailed.PreDetailedPlacement();
  
  //Before Detailed Placement
  if (arguments.count("pgp") == 1) {
    const string plot_name = arguments["pgp"].as<string>();
    ofstream plot(plot_name);
    database.Plot(plot, "Before Detailed Placement");
  }
  
  clock_t start,finish;
  double Runtime=0.0;
  start=clock();
  detailed.PreSATMethod();
  finish=clock();
  Runtime=(double)(finish-start)/CLOCKS_PER_SEC;
  cout<<"===> Pre SAT Method Time: "<<Runtime<<"..."<<endl;
  cout<<"============================================================"<<endl;
  //system("cd");
  //system("cd final/open-wbo/open-wbo/");
  //system("./open-wbo ~/final/lsv/pre/input > ~/final/lsv/post/output");
  start=clock();
  detailed.PostSATMethod();
  finish=clock();
  Runtime=(double)(finish-start)/CLOCKS_PER_SEC;
  cout<<"===> Post SAT Method Time: "<<Runtime<<"..."<<endl;
  cout<<"============================================================"<<endl;
  /*clock_t start,finish;
  double Runtime=0.0;
  start=clock();
  detailed.DPMethod();
  finish=clock();
  Runtime=(double)(finish-start)/CLOCKS_PER_SEC;
  cout<<"===> Dynamic Programming Time: "<<Runtime<<"..."<<endl;
  cout<<"============================================================"<<endl;*/
  //const string plot_name1 = arguments["pgp"].as<string>();

  const double total_displacement = database.ComputeTotalDisplacement();
  
  //After Detailed Placement
  if (arguments.count("plg") == 1) {
    const string plot_name = arguments["plg"].as<string>();
    ofstream plot(plot_name);
    database.Plot(plot,
                  "After Detailed Placement" + to_string(total_displacement));
  }

 
  //parser.OutputDef(global_placed_def_name, legalized_def_name);

  return 0;
}
