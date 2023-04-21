add_library(simdjson SHARED simdjson.cpp Public/simdjson.h)
target_include_directories(simdjson PUBLIC Public)