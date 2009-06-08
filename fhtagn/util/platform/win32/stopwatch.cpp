/**
 * $Id: template.h 197 2008-11-02 12:02:37Z unwesen $
 *
 * This file is part of the Fhtagn! C++ Library.
 * Copyright (C) 2009 Jens Finkhaeuser <unwesen@users.sourceforge.net>.
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

#include <fhtagn/util/stopwatch.h>

#include <stdexcept>

namespace fhtagn {
namespace util {

stopwatch::times_t
stopwatch::get_absolute_times()
{
  // Grab times from runtime
  FILETIME starttime;
  FILETIME endtime;
  FILETIME systime;
  FILETIME usertime;

  if (0 == GetProcessTimes(GetCurrentProcess(), &starttime, &endtime, &systime,
        &usertime))
  {
    LPVOID buf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &buf,
        0, NULL);

    std::runtime_error err = std::runtime_error(static_cast<char *>(buf));
    LocalFree(buf);
    throw err;
  }

  // Convert wall time
  ULARGE_INTEGER wall_start;
  ::memcpy(&wall_start, &starttime, sizeof(FILETIME));
  wall_start.QuadPart /= 10L;

  ULARGE_INTEGER wall_end;
  ::memcpy(&wall_end, &endtime, sizeof(FILETIME));
  wall_end.QuadPart /= 10L;

  usec_t wall_time = wall_end.QuadPart - wall_start.QuadPart;

  // Convert system & user times
  ULARGE_INTEGER sys;
  ::memcpy(&sys, &systime, sizeof(FILETIME));
  usec_t sys_time = sys.QuadPart / 10L;

  ULARGE_INTEGER user;
  ::memcpy(&user, &usertime, sizeof(FILETIME));
  usec_t user_time = user.QuadPart / 10L;

  return boost::make_tuple(wall_time, sys_time, user_time);
}

}} // namespace fhtagn::util
