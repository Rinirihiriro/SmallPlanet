#pragma once

#include <physfs.h>
#include <sstream>
#include <locale>

class PhysFSStream
{
public:
	std::istringstream stream;
	std::wistringstream wstream;

private:
	bool bLoad;

public:
	PhysFSStream();
	PhysFSStream(const std::string& zipfile, const std::string datafile, const bool widechar=false);

	void load(const std::string& zipfile, const std::string datafile);
	void load_w(const std::string& zipfile, const std::string datafile);
	bool is_load();

	void imbue_w(const std::locale& loc);
	void clear();
};