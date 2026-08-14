#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#undef min
#undef max
#undef add

#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

#pragma comment(lib, "Mfplat.lib")
#include <mfapi.h>

// Namespaces
using namespace std::literals;
using namespace string_literals;

#include "IW4.hpp"
