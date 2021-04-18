#pragma once

#ifdef VQS_EXPORT_DEF
#define EXPORT_VQS __declspec(dllexport)
#else
#define EXPORT_VQS __declspec(dllimport)
#endif
