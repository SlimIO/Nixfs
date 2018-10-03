#include "napi.h"
#include <sstream>
#include <iostream>
#include <errno.h>
#include <string>

#if __FreeBSD__
#include <sys/param.h>
#include <sys/mount.h>
#else
#include <sys/statfs.h>
#include <mntent.h>
#endif

using namespace Napi;
using namespace std;

/**
 * Get UNIX mounted entries
 * 
 * @header: mntent.h
 * @doc: http://man7.org/linux/man-pages/man3/getmntent.3.html
 */
Value getMountedEntries(const CallbackInfo& info) {
    Env env = info.Env();
    unsigned i = 0;
    Array ret = Array::New(env);

    // On FreeBSD there is no mntent API (so we read /etc/fstab directly!)
    #if __FreeBSD__
        char line[256], devName[255], dirName[255], type[50], options[255];
        unsigned int freq, passno;

        auto fd = fopen("/etc/fstab", "r");
        if (fd == NULL) {
            stringstream err;
            err << "failed to open /etc/fstab, error code: " << errno << endl;
            Error::New(env, err.str()).ThrowAsJavaScriptException();
            return env.Null();
        }

        // Remove first line of the file (which contains tab info).
        fgets(line, sizeof(line), fd);
        while (fgets(line, sizeof(line), fd) != NULL) {
            sscanf(line, "%s %s %s %s %u %u", devName, dirName, type, options, &freq, &passno);
            Object FS = Object::New(env);
            ret[i] = FS;
            FS.Set("dir", dirName);
            FS.Set("name", devName);
            FS.Set("type", type);
            FS.Set("freq", freq);
            FS.Set("passno", passno);
            FS.Set("opts", options);
            i++;
        }
        fclose(fd);
    #else
        struct mntent *mountedFS;
        FILE *fileHandle;

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
    #endif

    return ret;
}

/**
 * Get FileSystem stats
 * 
 * @header: sys/statfs.h
 * @doc: http://www.tutorialspoint.com/unix_system_calls/statfs.htm
 */
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

    // On FreeBSD the returned struct is not the same
    #if __FreeBSD__
        ret.Set("nameLen", Number::New(env, stat.f_namemax));
    #else
        Array fsid = Array::New(env, (size_t) 2);
        for (unsigned i = 0; i<2; i++) {
            fsid[i] = stat.f_fsid.__val[i];
        }
        ret.Set("fsid", fsid);
        ret.Set("nameLen", Number::New(env, stat.f_namelen));
    #endif

    return ret;
}

/**
 * Read /proc/diskstats
 * 
 * @doc: https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats
 */
Value getDiskStats(const CallbackInfo& info) {
    Env env = info.Env();
    char line[256], dev_name[100];
    int returnedElements;
    unsigned int ios_pgr, tot_ticks, rq_ticks, wr_ticks;
	unsigned long rd_ios, rd_merges_or_rd_sec, rd_ticks_or_wr_sec, wr_ios;
	unsigned long wr_merges, rd_sec_or_wr_ios, wr_sec;
    unsigned int major, minor;
    unsigned i = 0;

    auto fd = fopen("/proc/diskstats", "r");
    if (fd == NULL) {
        stringstream err;
        err << "failed to open /proc/diskstats, error code: " << errno << endl;
        Error::New(env, err.str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    Array ret = Array::New(env);
    while (fgets(line, sizeof(line), fd) != NULL) {
        returnedElements = sscanf(line, "%u %u %s %lu %lu %lu %lu %lu %lu %lu %u %u %u %u",
			   &major, &minor, dev_name,
			   &rd_ios, &rd_merges_or_rd_sec, &rd_sec_or_wr_ios, &rd_ticks_or_wr_sec,
			   &wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks);

        Object JSObject = Object::New(env);
        ret[i] = JSObject;
        if (returnedElements == 14) {
            JSObject.Set("devName", dev_name);
            JSObject.Set("major", major);
            JSObject.Set("minor", minor);
            JSObject.Set("rdIos", rd_ios);
            JSObject.Set("rdMergesOrRdSec", rd_merges_or_rd_sec);
            JSObject.Set("rdSecOrWrIos", rd_sec_or_wr_ios);
            JSObject.Set("rdTicksOrWrSec", rd_ticks_or_wr_sec);
            JSObject.Set("wrIos", wr_ios);
            JSObject.Set("wrMerges", wr_merges);
            JSObject.Set("wrSec", wr_sec);
            JSObject.Set("wrTicks", wr_ticks);
            JSObject.Set("iosPgr", ios_pgr);
            JSObject.Set("totTicks", tot_ticks);
            JSObject.Set("rqTicks", rq_ticks);
            i++;
        }
        else if(returnedElements == 7) {
            JSObject.Set("devName", dev_name);
            JSObject.Set("rdIos", rd_ios);
            JSObject.Set("rdMergesOrRdSec", rd_merges_or_rd_sec);
            JSObject.Set("wrIos", wr_ios);
            JSObject.Set("rdTicksOrWrSec", rd_ticks_or_wr_sec);
            i++;
        }
    }
    fclose(fd);

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
    exports.Set("getDiskStats", Function::New(env, getDiskStats));

    return exports;
}

// Export Nixfs module
NODE_API_MODULE(Nixfs, Init)
