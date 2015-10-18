#pragma once

#define NOMINMAX
#define _USE_MATH_DEFINES

#include <wrl.h>
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1effects_1.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>
#include <concrt.h>
#include <collection.h>
#include "App.xaml.h"

/* C++ headers */
#include <limits>
#include <cstdint>
#include <cmath>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <type_traits>
#include <set>
#include <unordered_set>
#include <list>

#pragma warning(disable:4244)
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/algorithm/string.hpp>
#pragma warning(default:4244)