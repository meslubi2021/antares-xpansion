// projet_benders.cpp : définit le point d'entrée pour l'application console.
//
#include "launcher.h"
#include "Worker.h"
#include "BendersOptions.h"
#include "BendersFunctions.h"


int main(int argc, char** argv)
{
	usage(argc);
	BendersOptions options(build_benders_options(argc, argv));
	options.print(std::cout);

	XPRSinit("");
	CouplingMap input;
	build_input(options, input);
	int i(0);
	std::vector<DblVector> name_rhs(input.size());
	Str2Int id_name;

	size_t n_rows(-1);
	for (auto const & kvp : input) {
		std::string problem_name(options.INPUTROOT + PATH_SEPARATOR + kvp.first);
		std::cout << problem_name << std::endl;
		XPRSprob prob;
		XPRScreateprob(&prob);
		XPRSsetcbmessage(prob, optimizermsg, NULL);
		XPRSsetintcontrol(prob, XPRS_OUTPUTLOG, XPRS_OUTPUTLOG_NO_OUTPUT);
		XPRSreadprob(prob, problem_name.c_str(), "");
		if (kvp.first != options.MASTER_NAME) {
			StandardLp lpData(prob);
			DblVector const & rhs = std::get<Attribute::DBL_VECTOR>(lpData._data)[DblVectorAttribute::RHS];
			name_rhs[i] = rhs;
			id_name[kvp.first] = i;
			n_rows = rhs.size();
		}

		++i;
		if (i > 5)break;
		XPRSdestroyprob(prob);
	}
	XPRSfree();
	std::ofstream file("toto.csv");
	for (auto const & kvp : id_name) {
		file << kvp.first << ";";
	}
	file << std::endl;
	for (size_t r(0); r < n_rows; ++r) {
		for (auto const & kvp : id_name) {
			file << name_rhs[kvp.second][r] << ";";
		}
		file << std::endl;
	}
	file.close();
	return 0;
}