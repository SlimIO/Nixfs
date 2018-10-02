#include "napi.h"
#include <sstream>
#include <iostream>
#include <errno.h>
#include <string>
#include <sys/statfs.h>
#include <mntent.h>

using namespace Napi;
using namespace std;

Value getMountedEntries(const CallbackInfo& info) {
    Env env = info.Env();
    struct mntent *mountedFS;
    FILE *fileHandle;
    unsigned i = 0;

    Array ret = Array::New(env);
    fileHandle = setmntent(_PATH_MOUNTED, "r");
    while ((mountedFS = getmntent(fileHandle))) {
        Object FS = Object::New(env);
        ret[i] = FS;
        FS.Set("dir", mountedFS->mnt_dir);
        FS.Set("name", mountedFS->mnt_fsname);
        FS.Set("type", mountedFS->mnt_type);
        FS.Set("freq", mountedFS->mnt_freq);
        FS.Set("passno", mountedFS->mnt_passno);
        FS.Set("opts", mountedFS->mnt_opts);
        i++;
    }
    endmntent(fileHandle);

    return ret;
}

Value getStatFS(const CallbackInfo& info) {
    Env env = info.Env();
    struct statfs stat;

    // Check argument length!
    if (info.Length() < 1) {
        Error::New(env, "Wrong number of argument provided!").ThrowAsJavaScriptException();
        return env.Null();
    }

    // callback should be a Napi::Function
    if (!info[0].IsString()) {
        Error::New(env, "argument fsPath should be a String!").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Retrieve FS Path!
    string fsPath = info[0].As<String>().Utf8Value();

    if (statfs(fsPath.c_str(), &stat) == -1) {
        stringstream err;
        err << "statfs failed with error code: " << errno << endl;
        Error::New(env, err.str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    Object ret = Object::New(env);
    ret.Set("type", Number::New(env, stat.f_type));
    ret.Set("bsize", Number::New(env, stat.f_bsize));
    ret.Set("blocks", Number::New(env, stat.f_blocks));
    ret.Set("bfree", Number::New(env, stat.f_bfree));
    ret.Set("bavail", Number::New(env, stat.f_bavail));
    ret.Set("files", Number::New(env, stat.f_files));
    ret.Set("ffree", Number::New(env, stat.f_ffree));
    ret.Set("availableSpace", Number::New(env, stat.f_bsize * stat.f_bavail));
    Array fsid = Array::New(env, (size_t) 2);
    for (unsigned i = 0; i<2; i++) {
        fsid[i] = stat.f_fsid.__val[i];
    }
    ret.Set("fsid", fsid);
    ret.Set("nameLen", Number::New(env, stat.f_namelen));

    return ret;
}

/*
 * Initialize Node.JS Binding exports
 * 
 * @header: napi.h
 */
Object Init(Env env, Object exports) {
    exports.Set("getMountedEntries", Function::New(env, getMountedEntries));
    exports.Set("getStatFS", Function::New(env, getStatFS));

    return exports;
}

// Export Nixfs module
NODE_API_MODULE(Nixfs, Init)
