#include <string>

#include "format.h"

using std::string;


inline string FormatTime(const int number){
	if (number < 10)
      return "0" + std::to_string(number);
  return std::to_string(number);
}


// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

string Format::ElapsedTime(long seconds) {
	const int hour = seconds / 3600;
  	const int second = seconds % 60;
  	const int minute = (seconds - hour * 3600 - second) / 60;
  
  	return (FormatTime(hour) + ":" + FormatTime(minute) + ":" + FormatTime(second));
} 


//this is the solution I got from Udacity Mentor
/*
string Format::ElapsedTime(long s) {
  std::chrono::seconds seconds{s};

  // return std::chrono::format("%T", seconds); // in C++20 :-)

  std::chrono::hours hours =
      std::chrono::duration_cast<std::chrono::hours>(seconds);

  seconds -= std::chrono::duration_cast<std::chrono::seconds>(hours);

  std::chrono::minutes minutes =
      std::chrono::duration_cast<std::chrono::minutes>(seconds);

  seconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

  std::stringstream ss{};

  ss << std::setw(2) << std::setfill('0') << hours.count()     // HH
     << std::setw(1) << ":"                                    // :
     << std::setw(2) << std::setfill('0') << minutes.count()   // MM
     << std::setw(1) << ":"                                    // :
     << std::setw(2) << std::setfill('0') << seconds.count();  // SS

  return ss.str();
} 
*/
