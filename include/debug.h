#pragma once
#include <Arduino.h>


#define LOG_LEVEL_NONE   0
#define LOG_LEVEL_ERROR  1
#define LOG_LEVEL_WARN   2
#define LOG_LEVEL_INFO   3


#ifndef LOG_LEVEL
  #define LOG_LEVEL LOG_LEVEL_INFO
#endif


#define LOG_TIMESTAMP()  Serial.printf("[%lu] ", millis())


#if LOG_LEVEL >= LOG_LEVEL_ERROR
  #define LOG_ERROR(fmt, ...)  \
    do { LOG_TIMESTAMP(); Serial.printf("❌ [ERROR] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
  #define LOG_ERROR(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
  #define LOG_WARN(fmt, ...)  \
    do { LOG_TIMESTAMP(); Serial.printf("⚠️ [WARN ] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
  #define LOG_WARN(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
  #define LOG_INFO(fmt, ...)  \
    do { LOG_TIMESTAMP(); Serial.printf("ℹ️ [INFO ] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
  #define LOG_INFO(fmt, ...)
#endif
