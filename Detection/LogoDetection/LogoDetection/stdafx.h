#pragma once

#ifndef STDAFX_H
#define STDAFX_H

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>

using namespace std;
using namespace cv;
using namespace boost::filesystem;
using namespace boost::algorithm;

#endif