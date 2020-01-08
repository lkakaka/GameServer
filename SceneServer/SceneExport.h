#pragma once
#ifdef SCENE_EXPORT
#ifndef SCENE_API
#define SCENE_API __declspec(dllexport)
#endif
#else
#define SCENE_API
#endif