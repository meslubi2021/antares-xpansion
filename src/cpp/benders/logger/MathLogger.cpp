#include "logger/MathLogger.h"

#include <iomanip>
#include <sstream>

double getDurationNotSolving(double iteration, double master,
                             double subproblems) {
  return iteration - master - subproblems;
}

void MathLoggerBase::setHeadersList() {
  auto type = HeadersType();
  HeadersManager headers_manager(type, BENDERSMETHOD::BENDERS);
  MathLogger::setHeadersList(headers_manager.headers_list);
}

void MathLogger::setHeadersList(const std::vector<std::string>& headers) {
  headers_.clear();
  headers_ = headers;
}

void PrintData(LogDestination& log_destination,
               const CurrentIterationData& data, const HEADERSTYPE& type,
               const BENDERSMETHOD& method) {
  log_destination << data.it;
  log_destination << std::scientific << std::setprecision(10) << data.lb;
  if (method == BENDERSMETHOD::BENDERS) {
    log_destination << std::scientific << std::setprecision(10) << data.ub;
    log_destination << std::scientific << std::setprecision(10) << data.best_ub;
    log_destination << std::scientific << std::setprecision(2)
                    << data.best_ub - data.lb;
    log_destination << std::scientific << std::setprecision(2)
                    << (data.best_ub - data.lb) / data.best_ub;
  }
  log_destination << data.min_simplexiter;
  log_destination << data.max_simplexiter;
  if (type == HEADERSTYPE::LONG || method == BENDERSMETHOD::BENDERSBYBATCH) {
    log_destination << data.number_of_subproblem_solved;
  }
  if (type == HEADERSTYPE::LONG) {
    log_destination << data.cumulative_number_of_subproblem_solved;
  }

  log_destination << std::setprecision(2) << data.iteration_time;
  log_destination << std::setprecision(2) << data.timer_master;
  log_destination << std::setprecision(2) << data.subproblems_walltime;

  if (type == HEADERSTYPE::LONG) {
    log_destination << std::setprecision(2)
                    << data.subproblems_cumulative_cputime;
    log_destination << std::setprecision(2)
                    << getDurationNotSolving(data.iteration_time,
                                             data.timer_master,
                                             data.subproblems_walltime);
  }
  log_destination << std::endl;
}
void MathLoggerBase::Print(const CurrentIterationData& data) {
  PrintData(LogsDestination(), data, HeadersType(), BENDERSMETHOD::BENDERS);
}

void MathLoggerBendersByBatch::setHeadersList() {
  auto type = HeadersType();
  HeadersManager headers_manager(type, BENDERSMETHOD::BENDERSBYBATCH);

  MathLogger::setHeadersList(headers_manager.headers_list);
}

void MathLoggerBendersByBatch::Print(const CurrentIterationData& data) {
  PrintData(LogsDestination(), data, HeadersType(),
            BENDERSMETHOD::BENDERSBYBATCH);
}

MathLoggerFile::MathLoggerFile(const BENDERSMETHOD& method,
                               const std::filesystem::path& filename,
                               std::streamsize width)
    : MathLoggerImplementation(method, filename, width, HEADERSTYPE::LONG) {}

void MathLoggerFile::display_message(const std::string& msg) {
  // keep empty
}