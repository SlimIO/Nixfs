#include "napi.h"

using namespace Napi;

/*
 * Initialize Node.JS Binding exports
 * 
 * @header: napi.h
 */
Object Init(Env env, Object exports) {

    return exports;
}

// Export Nixfs module
NODE_API_MODULE(Nixfs, Init)
