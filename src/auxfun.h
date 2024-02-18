#pragma once
#include "datatype.h"

bool	sort_qstringpair_secondgreater(pair<QString, int> p1, pair<QString, int> p2);

bool getFiles(std::string file_path, std::vector<std::string>& file_names);

bool	generate_output_directory(QString allDir);