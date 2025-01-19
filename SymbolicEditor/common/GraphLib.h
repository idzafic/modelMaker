#pragma once
#ifdef NATGRAPH_EXPORTS
    #ifdef MU_WINDOWS
        #define GRAPH_API __declspec(dllexport)
    #else
        #define GRAPH_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef MU_WINDOWS
        #define GRAPH_API __declspec(dllimport)
    #else
        #define GRAPH_API
    #endif
#endif
