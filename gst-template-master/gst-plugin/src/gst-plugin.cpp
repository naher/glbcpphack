// gst-plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "gstplugin.h"

#include <ctime>
#include <string>
#include <sstream>

//#include <chrono>

using namespace std;


GstFlowReturn dofilter(GstPad * pad, GstBuffer * buf) 
{
  /*
  typedef std::chrono::system_clock Clock;

  Clock::time_point t = Clock::now();
  auto seconds = std::duration_cast<std::chrono::seconds>(t);
  auto fraction = t - s;
  time_t tt = Clock::to_time_t(t);
  */

  stringstream sstream; sstream << "I'm plugged, therefore I'm in, time " << time(0) << "\n";
  g_print(sstream.str().c_str());
  return gst_pad_push (pad, buf);
}
