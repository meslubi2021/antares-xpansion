"""
    Class to work on config
"""

import os
import sys

from pathlib import Path

import re

from antares_xpansion.general_data_reader import GeneralDataIniReader
from antares_xpansion.input_checker import check_candidates_file, check_options
from antares_xpansion.xpansion_study_reader import XpansionStudyReader

import functools

print = functools.partial(print, flush=True)


class ConfigLoader:
    """
        Class to control the execution of the optimization session
    """

    def __init__(self, config):
        """
            Initialise driver with a given antaresXpansion configuration,
            the system platform and parses the arguments
            :param config: configuration to use for the optimization
            :type config: XpansionConfig object
        """
        self.platform = sys.platform
        self.config = config
        self.simulation_name = self.config.simulation_name

        self.candidates_list = []
        self._verify_settings_ini_file_exists()

        self.nb_active_years = GeneralDataIniReader(Path(self.general_data())).get_nb_activated_year()

        self.options = self._get_options_from_settings_inifile()

        self.check_candidates_file_format()
        self.check_settings_file_format()

    def _verify_settings_ini_file_exists(self):
        if not os.path.isfile(self._get_settings_ini_filepath()):
            print('Missing file : %s was not retrieved.' % self._get_settings_ini_filepath())
            sys.exit(1)

    def _get_options_from_settings_inifile(self):
        with open(self._get_settings_ini_filepath(), 'r') as file_l:
            options = dict(
                {line.strip().split('=')[0].strip(): line.strip().split('=')[1].strip()
                 for line in file_l.readlines() if line.strip()})
        return options

    def check_candidates_file_format(self):
        if not os.path.isfile(self.candidates_ini_filepath()):
            print('Missing file : %s was not retrieved.' % self.candidates_ini_filepath())
            sys.exit(1)

        check_candidates_file(self)

    def check_settings_file_format(self):
        check_options(self.options)
        self._verify_solver()
        self._verify_yearly_weights_consistency()
        self._verify_additional_constraints_file()

    def antares_output(self):
        """
            returns path to antares output data directory
        """
        return os.path.normpath(os.path.join(self.data_dir(), self.config.OUTPUT))

    def exe_path(self, exe):
        """
            prefixes the input exe with the install directory containing the binaries

            :param exe: executable name

            :return: path to specified executable
        """
        return os.path.normpath(os.path.join(self.config.install_dir, exe))

    def data_dir(self):
        """
            returns path to the data directory
        """
        return self.config.data_dir

    def weight_file_name(self):
        return self.options.get('yearly-weights', self.config.settings_default["yearly-weights"])

    def general_data(self):
        """
            returns path to general data ini file
        """
        return os.path.normpath(os.path.join(self.data_dir(),
                                             self.config.SETTINGS, self.config.GENERAL_DATA_INI))

    def _get_settings_ini_filepath(self):
        """
            returns path to setting ini file
        """
        return self._get_path_from_file_in_xpansion_dir(self.config.SETTINGS_INI)

    def candidates_ini_filepath(self):
        """
            returns path to candidates ini file
        """
        return self._get_path_from_file_in_xpansion_dir(self.config.CANDIDATES_INI)

    def _get_path_from_file_in_xpansion_dir(self, filename):
        return os.path.normpath(os.path.join(self.data_dir(), self.config.USER,
                                             self.config.EXPANSION, filename))

    def capacity_file(self, filename):
        """
            returns path to input capacity file
        """
        return os.path.normpath(os.path.join(self.data_dir(), self.config.USER,
                                             self.config.EXPANSION, self.config.CAPADIR, filename))

    def weights_file_path(self):
        """
            returns the path to a yearly-weights file

            :return: path to input yearly-weights file
        """

        yearly_weights_filename = self.weight_file_name()
        if yearly_weights_filename:
            return self._get_path_from_file_in_xpansion_dir(yearly_weights_filename)
        else:
            return ""

    def optimality_gap(self):
        """
            prints and returns the optimality gap read from the settings file

            :return: gap value or 0 if the gap is set to -Inf
        """
        optimality_gap_str = self.options.get('optimality_gap',
                                              self.config.settings_default["optimality_gap"])
        assert not '%' in optimality_gap_str
        return float(optimality_gap_str) if optimality_gap_str != '-Inf' else 0

    def max_iterations(self):
        """
            prints and returns the maximum iterations read from the settings file

            :return: max iterations value or -1 if the parameter is is set to +Inf
        """
        max_iterations_str = self.options.get('max_iteration',
                                              self.config.settings_default["max_iteration"])
        assert not '%' in max_iterations_str
        return float(max_iterations_str) if (
                (max_iterations_str != '+Inf') and (max_iterations_str != '+infini')) else -1

    def additional_constraints(self):
        """
            returns path to additional constraints file
        """
        additional_constraints_filename = self.options.get("additional-constraints",
                                                           self.config.settings_default["additional-constraints"])

        if additional_constraints_filename == "":
            return ""
        return self._get_path_from_file_in_xpansion_dir(additional_constraints_filename)

    def simulation_lp_path(self):
        return self._simulation_lp_path()

    def _simulation_lp_path(self):
        lp_path = os.path.normpath(os.path.join(self.simulation_output_path(), 'lp'))
        return lp_path

    def _verify_yearly_weights_consistency(self):
        if self.weight_file_name():
            try:
                XpansionStudyReader.check_weights_file(self.weights_file_path(), self.nb_active_years)
            except XpansionStudyReader.BaseException as e:
                print(e)
                sys.exit(1)

    def _verify_additional_constraints_file(self):
        if self.options.get('additional-constraints', "") != "":
            additional_constraints_path = self.additional_constraints()
            if not os.path.isfile(additional_constraints_path):
                print('Illegal value: %s is not an existent additional-constraints file'
                      % additional_constraints_path)
                sys.exit(1)

    def _verify_solver(self):
        try:
            XpansionStudyReader.check_solver(self.options.get('solver', ""), self.config.AVAILABLE_SOLVER)
        except XpansionStudyReader.BaseException as e:
            print(e)
            sys.exit(1)

    def simulation_output_path(self) -> Path:
        if (self.simulation_name == "last"):
            self._set_last_simulation_name()
        return Path(os.path.normpath(os.path.join(self.antares_output(), self.simulation_name)))

    def set_options_for_benders_solver(self):
        return self._set_options_for_benders_solver()

    def _set_options_for_benders_solver(self):
        """
            generates a default option file for the solver
        """
        # computing the weight of slaves
        options_values = self.config.options_default
        options_values["SLAVE_WEIGHT_VALUE"] = str(self.nb_active_years)
        options_values["GAP"] = self.optimality_gap()
        options_values["MAX_ITERATIONS"] = self.max_iterations()
        options_values["SOLVER_NAME"] = XpansionStudyReader.convert_study_solver_to_option_solver(
            self.options.get('solver', "Cbc"))
        if self.weight_file_name():
            options_values["SLAVE_WEIGHT"] = self.weight_file_name()
        # generate options file for the solver
        options_path = os.path.normpath(os.path.join(self._simulation_lp_path(), self.config.OPTIONS_TXT))
        with open(options_path, 'w') as options_file:
            options_file.writelines(["%30s%30s\n" % (kvp[0], kvp[1])
                                     for kvp in options_values.items()])

    def _set_last_simulation_name(self):
        """
            return last simulation name    
        """

        # simulation name folder YYYYMMDD-HHMMeco
        classic_simulation_name_regex = re.compile(
            "^\d{4}(0[1-9]|1[0-2])(0[1-9]|[12][0-9]|3[01])-([0-1]?[0-9]|2[0-3])[0-5][0-9]eco([-][-0-9]+?)?$")

        simulations_list = []
        for file in os.listdir(self.antares_output()):
            if (os.path.isdir(os.path.normpath(os.path.join(self.antares_output(), file))) and re.fullmatch(
                    classic_simulation_name_regex, file)):
                simulations_list.append(file)
        sorted_simulations_list = sorted(simulations_list)
        if len(sorted_simulations_list) == 0:
            msg = f"no suitable simulation directory found in {self.antares_output()}, simulation directory name must be in this format: YYYYMMDD-HHMMeco "
            raise XpansionStudyReader.NoSimulationDirectory(msg)
        self.simulation_name = sorted_simulations_list[-1]