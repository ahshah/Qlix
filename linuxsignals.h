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

  sigprocmask(SIG_UNBLOCK, &previous_set, NULL);
  exit(1);
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

