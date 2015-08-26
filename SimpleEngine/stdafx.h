#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#pragma warning( disable : 4005 )

#include <d3dx11.h>

#include <xnamath.h>
#include <dxerr.h>

#include <cassert>
#include <ctime>

#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <utility>
#include <bitset>

#include "MathHelper.h"
#include "LightHelper.h"
#include "d3dx11Effect.h"
#include "d3dUtil.h"


using namespace std;