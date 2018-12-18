// This is an example of how to to trap Cntrl-C in a cross-platform manner
// it creates a simple REPL event loop and shows how to interrupt it.

#include <csignal>
#include <cstdlib>

// This global is needed for communication between the signal handler
// and the rest of the code. This atomic integer counts the number of times
// Cntl-C has been pressed by not reset by the REPL code.
volatile sig_atomic_t global_status_flag = 0;

// *****************************************************************************
// install a signal handler for Cntl-C on Windows
// *****************************************************************************
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>

// this function is called when a signal is sent to the process
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {

  switch (fdwCtrlType) {
  case CTRL_C_EVENT: // handle Cnrtl-C
    // if not reset since last call, exit
    if (global_status_flag > 0) { 
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
    return TRUE;

  default:
    return FALSE;
  }
}

// install the signal handler
inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }
// *****************************************************************************

// *****************************************************************************
// install a signal handler for Cntl-C on Unix/Posix
// *****************************************************************************
#elif defined(__APPLE__) || defined(__linux) || defined(__unix) ||             \
    defined(__posix)
#include <unistd.h>

// this function is called when a signal is sent to the process
void interrupt_handler(int signal_num) {

  if(signal_num == SIGINT){ // handle Cnrtl-C
    // if not reset since last call, exit
    if (global_status_flag > 0) {
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
  }
}

// install the signal handler
inline void install_handler() {

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = interrupt_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif
// *****************************************************************************
