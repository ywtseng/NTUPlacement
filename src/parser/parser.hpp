#ifndef PARSER_HPP
#define PARSER_HPP

#include "../database/database.hpp"

#include <def/defrReader.hpp>
#include <lef/lefrReader.hpp>

#include <fstream>
#include <string>
class Parser {
 public:
  Parser(Database& database, const std::string& tech_lef_name,
         const std::string& cell_lef_name,
         const std::string& global_placed_def_name,
         const std::string& placement_constraints_name);

  void OutputDef(const std::string& global_placed_def_name,
                 const std::string& output_def_name);

 private:
  void ParseTechLef(const std::string& tech_lef_name);
  void ParseCellLef(const std::string& cell_lef_name);
  void ParseGlobalPlacedDef(const std::string& global_placed_def_name);
  void ParsePlacementConstraints(const std::string& placement_constraints_name);

  Database& database_;
};

#endif
