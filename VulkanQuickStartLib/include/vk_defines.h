/*

This file is part of the VulkanQuickStart Project.

	The VulkanQuickStart Project is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	The VulkanQuickStart Project is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	This link provides the exact terms of the GPL license <https://www.gnu.org/licenses/>.

	The author's interpretation of GPL 3 is that if you receive money for the use or distribution of the TriMesh Library or a derivative product, GPL 3 no longer applies.

	Under those circumstances, the author expects and may legally pursue a reasoble share of the income. To avoid the complexity of agreements and negotiation, the author makes
	no specific demands in this regard. Compensation of roughly 1% of net or $5 per user license seems appropriate, but is not legally binding.

	In lay terms, if you make a profit by using the VulkanQuickStart Project (violating the spirit of Open Source Software), I expect a reasonable share for my efforts.

	Robert R Tipton - Author

	Dark Sky Innovative Solutions http://darkskyinnovation.com/

*/

#pragma once

#include <vk_exports.h>
#include <tm_defines.h>

#ifdef _HAS_ITERATOR_DEBUGGING
#undef _HAS_ITERATOR_DEBUGGING
#endif

#define _HAS_ITERATOR_DEBUGGING 0
#define USE_EIGEN_VECTOR3 0

#ifdef _GCC
#define GCC_CLASS class
#else
#define GCC_CLASS
#endif // _GCC

#include <vector> // Need to include to expose size_t
#include <stdexcept>
#include <vulkan/vulkan_core.h>

constexpr size_t K_BYTE = 1024;
constexpr size_t M_BYTE = 1024 * K_BYTE;

/* 
	DEV_MAX_BUF_SIZE is a development safety barrier. 
	
	On windows systems, if you pass an un initialized value for size when allocating a buffer (and it's huge 64 bit)
	the OS will try to allocate it and over load your swap file. 
	This causes your system to lockup.
	This causes you to force powerdown your box before it fills the universe.
	This causes Windows to very carefully down size your swap file on restart.
	Which causes the operating system to suck up all disk resources for nearly one hour.

	As you may guess, this happened and I lost a good chunk of an afternoon.

*/
constexpr size_t DEV_MAX_BUF_SIZE = 512 * M_BYTE;

#ifndef stm1
#define stm1 0xffffffffffffffff
#endif

