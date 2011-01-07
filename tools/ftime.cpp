/**
 * $Id$
 *
 * This file is part of the Fhtagn! C++ Library.
 * Copyright (C) 2010,2011 Jens Finkhaeuser <unwesen@users.sourceforge.net>.
 *
 * Author: Jens Finkhaeuser <unwesen@users.sourceforge.net>
 *
 * This program is licensed as free software for personal, educational or
 * other non-commerical uses: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, licenses for commercial purposes are available as well.
 * Please send your enquiries to the copyright holder's address above.
 **/

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <string>

#include <fhtagn/fhtagn.h>
#include <fhtagn/util/stopwatch.h>

#include <boost/program_options.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <boost/format.hpp>

namespace po = boost::program_options;
namespace acc = boost::accumulators;


namespace {

void usage(po::options_description const & desc)
{
  std::cout << "Usage: ftime [options] child [ child_arg [...]]" << std::endl << std::endl;
  std::cout << desc << std::endl;
}



} // anonymous namespace



int main(int argc, char **argv)
{
  // *** Program options

  po::options_description visible(
    "Precisely time the execution time of a program.\n\n"
    "The `ftime' command works much like the UNIX `time(1)' command, but to lend\n"
    "the timing more precision, the average run time over a number of runs is\n"
    "reported. Timing still includes forking the child process, though.\n\n"
    "Command line arguments"
  );

  boost::uint32_t runs = 0;
  bool verbose = false;

  visible.add_options()
    ("help", "Prints this help text and exits.")
    ("runs", po::value<boost::uint32_t>(&runs)->default_value(20),
        "Number of times the program to be timed is run.")
    ("verbose", po::value<bool>(&verbose)->zero_tokens(),
        "Be verbose about the output.")
  ;

  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("child_args", po::value<std::vector<std::string> >(),
        "Child to call and arguments for the child call.")
  ;

  po::options_description all("Allowed options");
  all.add(visible).add(hidden);

  po::positional_options_description pos;
  pos.add("child_args", -1);

  bool show_help = false;
  po::variables_map vm;
  try {
    po::store(
        po::command_line_parser(argc, argv)
          .options(all)
          .allow_unregistered()
          .positional(pos)
          .run(),
        vm);
    po::notify(vm);
  } catch (po::error const & ex) {
    std::cerr << "ERROR: " << ex.what() << std::endl;
    show_help = true;
  }

  if (show_help || vm.count("help")) {
    usage(visible);
    std::exit(1);
  }

  std::vector<std::string> child_args;
  try {
    child_args = vm["child_args"].as<std::vector<std::string> >();
  } catch (boost::bad_any_cast const & ex) {
    std::cerr << "ERROR: must provide a child executable." << std::endl;
    usage(visible);
    std::exit(2);
  }

  if (verbose) {
    std::cout << "Average run time over " << runs << " runs." << std::endl;
  }

  // *** Prepare for execution of child process.

  // Convert vector<string> to char ** XXX Note that we don't allocate memory
  // for the actual strings, so don't delete child_args before args.
  char const ** args = new char const *[child_args.size() + 1];
  args[child_args.size()] = 0;
  for (std::size_t i = 0 ; i < child_args.size(); ++i) {
    args[i] = child_args[i].c_str();
  }


  // *** Run the child

  typedef acc::accumulator_set<
    fhtagn::util::stopwatch::usec_t,
    acc::features<
      acc::tag::mean,
      acc::tag::median,
      acc::tag::variance
    >
  > accumulator_t;
  accumulator_t sys_time;
  accumulator_t user_time;

  fhtagn::util::stopwatch stopwatch;

  for (std::size_t run = 0 ; run < runs ; ++run) {
    pid_t pid = fork();
    if (pid == 0) {
      // *** child
      execvp(args[0], (char * const *) args);
      std::cerr << "Error executing child: " << strerror(errno) << std::endl;
      std::exit(4);
    }
    else if (pid > 0) {
      // *** parent
      typedef void (*sig_t) (int);
      sig_t interrupt_sig = signal(SIGINT, SIG_IGN);
      sig_t quit_sig = signal(SIGQUIT, SIG_IGN);

      int status;
      struct ::rusage child_usage;
      wait4(pid, &status, 0, &child_usage);
      if (status) {
        std::exit(5);
      }

      sys_time((static_cast<fhtagn::util::stopwatch::usec_t>(child_usage.ru_stime.tv_sec) * 1000000)
        + child_usage.ru_stime.tv_usec);
      user_time((static_cast<fhtagn::util::stopwatch::usec_t>(child_usage.ru_utime.tv_sec) * 1000000)
        + child_usage.ru_utime.tv_usec);

      signal(SIGINT, interrupt_sig);
      signal(SIGQUIT, quit_sig);
    }
    else {
      // *** error
      std::cerr << "Could not fork child process." << std::endl;
      std::exit(3);
    }
  }

  // *** Results

  fhtagn::util::stopwatch::times_t times = stopwatch.get_times();

  std::cout << std::endl;
  std::cout << boost::format("Total wall time:     %10d.--- usec") % times.get<0>() << std::endl;
  std::cout << boost::format("Mean wall time:      %14.3f usec") % (times.get<0>() / double(runs)) << std::endl;
  std::cout << std::endl;

  std::cout << boost::format("Mean system time:    %14.3f usec") % acc::mean(sys_time) << std::endl;
  std::cout << boost::format("Median system time:  %14.3f usec") % acc::median(sys_time) << std::endl;
  std::cout << boost::format("Standard deviation:  %14.3f usec") % sqrtl(acc::variance(sys_time)) << std::endl;

  std::cout << std::endl;

  std::cout << boost::format("Mean user time:      %14.3f usec") % acc::mean(user_time) << std::endl;
  std::cout << boost::format("Median user time:    %14.3f usec") % acc::median(user_time) << std::endl;
  std::cout << boost::format("Standard deviation:  %14.3f usec") % sqrtl(acc::variance(user_time)) << std::endl;

  std::exit(0);
}


