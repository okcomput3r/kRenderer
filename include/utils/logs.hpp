#ifndef OK_LOGS_H
#define OK_LOGS_H


#ifdef __DEBUG__
#define OK_DEBUG
#endif // DEBUG

#ifdef VULKAN_SDK
  std::cout << "vulkan sdk detected" << std::endl;

#endif
namespace Log::Warning {


}

namespace Log::Error {

}

namespace Log::Assert {

// MACRO DEFINITONS


// assert_paranoid
#define assert_paranoid(COND, MSG)                       \
  do {                                                   \
    if (COND) {                                          \
      Log::Assert::_assert_paranoid(__LINE__, MSG);      \
    }                                                    \
  } while (0)





#if defined (OK_DEBUG)
#define assert(COND, MSG) if(COND){_assert()}
#else
#define assert(COND, MSG) (void*)(0)
#endif // end assert




// FUNCTIONS DEFINITIONS

/***
 *
 * assert_paranoid() stops the execution of the program after displaying the 
 * error message that causes the crash.
 * Paranoid is designed to be the most strict kind of assert, assuming the code
 * will fail.
 *
 * @param line : int -> refers to the line where the error was found
 * @param msg : const char * -> message explaining the reason for the crash
 */
void _assert_paranoid(int line, const char* msg);

/***
 *
 * assert() stops the execution of the program if the check fails, and displays 
 * the error message.
 *
 * @param line : int -> refers to the line where the error was found
 * @param msg : const char * -> message explaining the reason for the crash
 */
void _assert(int line, const char* msg);


/***
 *
 * soft_assert() logs a warning message if the condition fails, but unlike assert() or
 * assert_paranoid(), it keeps the code running
 *
 * @param line : int -> refers to the line where the error was found
 * @param msg : const char * -> message explaining the reason for the crash
 */
void _soft_assert(int line, const char* msg);


} // end Log::Assert



#endif /* OK_LOGS_H */


// IMPLEMENTATION

//REMOVE WHEN DONE

#ifdef OK_LOGS_IMPL

void Log::Assert::_assert_paranoid(int line, const char* msg) {

}


#endif /* OK_LOGS_IMPL */

