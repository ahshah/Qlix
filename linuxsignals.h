/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This file is part of the Qlix project on http://berlios.de
 *
 *   This file may be used under the terms of the GNU General Public
 *   License version 2.0 as published by the Free Software Foundation
 *   and appearing in the file COPYING included in the packaging of
 *   this file.  
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License version 2.0 for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <signal.h>
#include <stdio.h>
extern MtpSubSystem _subSystem;

void handler(int sig)
{
  sigset_t set;
  sigset_t previous_set;
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGFPE);
  sigaddset(&set, SIGQUIT);
  sigaddset(&set, SIGSEGV);
  sigaddset(&set, SIGHUP);
  sigaddset(&set, SIGKILL);
  sigprocmask(SIG_BLOCK, &set,&previous_set);

  _subSystem.ReleaseDevices();
  printf("Ouch! - Qlix received signal %d\n", sig);

  struct sigaction act;
  act.sa_handler = SIG_DFL;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(sig, &act, 0);

  sigprocmask(SIG_UNBLOCK, &previous_set, NULL);
}

void installSignalHandlers()
{
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);
    sigaction(SIGFPE, &act, 0);
    sigaction(SIGQUIT, &act, 0);
    sigaction(SIGSEGV, &act, 0);
    sigaction(SIGHUP, &act, 0);
    sigaction(SIGABRT, &act, 0);
    sigaction(SIGKILL, &act, 0);
}
