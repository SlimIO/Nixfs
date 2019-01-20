#include "napi.h"
#include <sstream>
#include <iostream>
#include <errno.h>

#if __FreeBSD__
#include <sys/param.h>
#include <sys/mount.h>

struct statfs {
    char* devName;
    char* dirName;
    char* type;
    char* options;
    unsigned int freq;
    unsigned int passno;
};
#else
#include <sys/statfs.h>
#include <mntent.h>
#endif

using namespace Napi;
using namespace std;

/**
 * Asynchronous Worker
 * @name MountedEntriesWorker
 * @desc Get UNIX mounted entries
 * 
 * @header: errno.h
 * @header[linux]: mntent.h
 * @header[freebsd]: sys/param.h
 * @header[freebsd]: sys/mount.h
 * 
 * @doc: http://debian-facile.org/doc:systeme:fstab
 * @doc: http://man7.org/linux/man-pages/man3/getmntent.3.html
 */
class MountedEntriesWorker : public AsyncWorker {
    public:
        MountedEntriesWorker(Function& callback) : AsyncWorker(callback) {}
        ~MountedEntriesWorker() {}
    private:
        struct MountedEntry {
            string dir;
            string name;
            string type;
            unsigned int freq;
            unsigned int passno;
            vector<string> options;
        };
        vector<MountedEntry> entries;

    void Execute() {
        #if __FreeBSD__
            char line[256];

            auto fd = fopen("/etc/fstab", "r");
            if (fd == NULL) {
                return SetError("failed to open /etc/fstab");
            }

            statfs* fstat = (statfs*) malloc(sizeof(statfs));
            try {
                // Remove first line of the file (which contains tab info).
                fgets(line, sizeof(line), fd);

                while (fgets(line, sizeof(line), fd) != NULL) {
                    sscanf(line, "%s %s %s %s %u %u", 
                        fstat->devName, fstat->dirName, fstat->type, fstat->options, &fstat->freq, &fstat->passno);

                    vector<string> opts;
                    char* cA = strtok(fstat->options, ",");
                    while(cA) {
                        opts.push_back(string(cA));
                        cA = strtok(NULL, ",");
                    }

                    entries.push_back({
                        string(fstat->dirName),
                        string(fstat->devName),
                        string(fstat->type),
                        fstat->freq,
                        fstat->passno,
                        opts
                    });
                }
            }
            catch (const std::exception& e) {
                free(fstat);
                fclose(fd);

                return SetError(e.what());
            }

            free(fstat);
            fclose(fd);
        #else
            struct mntent *mountedFS;
            FILE *fileHandle;

            fileHandle = setmntent(_PATH_MOUNTED, "r");
            while ((mountedFS = getmntent(fileHandle))) {
                vector<string> opts;
                char* cA = strtok(mountedFS->mnt_opts, ",");
                while(cA) {
                    opts.push_back(string(cA));
                    cA = strtok(NULL, ",");
                }

                entries.push_back({
                    string(mountedFS->mnt_dir),
                    string(mountedFS->mnt_fsname),
                    string(mountedFS->mnt_type),
                    (unsigned int) mountedFS->mnt_freq,
                    (unsigned int) mountedFS->mnt_passno,
                    opts
                });
            }
            endmntent(fileHandle);
        #endif
    }

    void OnError(const Error& e) {
        stringstream error;
        error << e.what() << ", error code: " << errno << endl;
        Error::New(Env(), error.str()).ThrowAsJavaScriptException();
    }

    void OnOK() {
        HandleScope scope(Env());
        Array ret = Array::New(Env());

        for(size_t i = 0; i < entries.size(); i++) {
            MountedEntry mnt = entries[i];
            Object entry = Object::New(Env());
            ret[i] = entry;

            entry.Set("dir", mnt.dir);
            entry.Set("name", mnt.name);
            entry.Set("type", mnt.type);
            entry.Set("freq", mnt.freq);
            entry.Set("passno", mnt.passno);

            Array options = Array::New(Env());
            for (size_t j = 0; j < mnt.options.size(); j++) {
                options[j] = mnt.options[j];
            }
            entry.Set("options", options);
        }

        Callback().Call({Env().Null(), ret});
    }

};

/**
 * Get UNIX mounted entries (binding interface)
 */
Value getMountedEntries(const CallbackInfo& info) {
    Env env = info.Env();

    if (!info[0].IsFunction()) {
        Error::New(env, "argument callback must be a Function!").ThrowAsJavaScriptException();
        return env.Null();
    }

    Function cb = info[0].As<Function>();
    (new MountedEntriesWorker(cb))->Queue();
    
    return env.Undefined();
}

/**
 * Get the File System Name with the type id (long).
 * @doc: @doc: http://www.tutorialspoint.com/unix_system_calls/statfs.htm
 */
const char* getFileSystemType(long type) {
    switch(type) {
        case 0xadf5:
            return "ADFS_SUPER_MAGIC";
        case 0xADFF:
            return "AFFS_SUPER_MAGIC";
        case 0x42465331:
            return "BEFS_SUPER_MAGIC";
        case 0x1BADFACE:
            return "BFS_MAGIC";
        case 0xFF534D42:
            return "CIFS_MAGIC_NUMBER";
        case 0x73757245:
            return "CODA_SUPER_MAGIC";
        case 0x012FF7B7:
            return "COH_SUPER_MAGIC";
        case 0x28cd3d45:
            return "CRAMFS_MAGIC";
        case 0x1373:
            return "DEVFS_SUPER_MAGIC";
        case 0x00414A53:
            return "EFS_SUPER_MAGIC";
        case 0x137D:
            return "EXT_SUPER_MAGIC";
        case 0xEF51:
            return "EXT2_OLD_SUPER_MAGIC";
        case 0xEF53:
            return "EXT2_SUPER_MAGIC";
        case 0x4244:
            return "HFS_SUPER_MAGIC";
        case 0xF995E849:
            return "HPFS_SUPER_MAGIC";
        case 0x958458f6:
            return "HUGETLBFS_MAGIC";
        case 0x9660:
            return "ISOFS_SUPER_MAGIC";
        case 0x72b6:
            return "JFFS2_SUPER_MAGIC";
        case 0x3153464a:
            return "JFS_SUPER_MAGIC";
        case 0x137F:
            return "MINIX_SUPER_MAGIC";
        case 0x138F:
            return "MINIX_SUPER_MAGIC2";
        case 0x2468:
            return "MINIX2_SUPER_MAGIC";
        case 0x2478:
            return "MINIX2_SUPER_MAGIC2";
        case 0x4d44:
            return "MSDOS_SUPER_MAGIC";
        case 0x564c:
            return "NCP_SUPER_MAGIC";
        case 0x6969:
            return "NFS_SUPER_MAGIC";
        case 0x5346544e:
            return "NTFS_SB_MAGIC";
        case 0x9fa1:
            return "OPENPROM_SUPER_MAGIC";
        case 0x9fa0:
            return "PROC_SUPER_MAGIC";
        case 0x002f:
            return "QNX4_SUPER_MAGIC";
        case 0x52654973:
            return "REISERFS_SUPER_MAGIC";
        case 0x7275:
            return "ROMFS_MAGIC";
        case 0x517B:
            return "SMB_SUPER_MAGIC";
        case 0x012FF7B6:
            return "SYSV2_SUPER_MAGIC";
        case 0x012FF7B5:
            return "SYSV4_SUPER_MAGIC";
        case 0x01021994:
            return "TMPFS_MAGIC";
        case 0x15013346:
            return "UDF_SUPER_MAGIC";
        case 0x00011954:
            return "UFS_MAGIC";
        case 0x9fa2:
            return "USBDEVICE_SUPER_MAGIC";
        case 0xa501FCF5:
            return "VXFS_SUPER_MAGIC";
        case 0x012FF7B4:
            return "XENIX_SUPER_MAGIC";
        case 0x58465342:
            return "XFS_SUPER_MAGIC";
        case 0x012FD16D:
            return "_XIAFS_SUPER_MAGIC";
        default:
            return "N/A";
    }
}

/**
 * Asynchronous Worker
 * @name StatFSWorker
 * @desc Get (retrieve) a given FileSystem directory stats 
 * 
 * @header: sys/statfs.h
 * @header: errno.h
 * @doc: http://www.tutorialspoint.com/unix_system_calls/statfs.htm
 */
class StatFSWorker : public AsyncWorker {
    public:
        StatFSWorker(Function& callback, string fsPath) : AsyncWorker(callback), fsPath(fsPath) {}
        ~StatFSWorker() {}
    private:
        string fsPath;
        struct statfs stat;

    void Execute() {
        if (statfs(fsPath.c_str(), &stat) == -1) {
            stringstream error;
            error << "statsfs failed for path: " << fsPath << ", error code: " << errno << "\n";
            SetError(error.str());
        }
    }

    void OnOK() {
        HandleScope scope(Env());

        Object ret = Object::New(Env());
        ret.Set("typeId", stat.f_type);
        ret.Set("type", getFileSystemType(stat.f_type));
        ret.Set("bsize", stat.f_bsize);
        ret.Set("blocks", stat.f_blocks);
        ret.Set("bfree", stat.f_bfree);
        ret.Set("bavail", stat.f_bavail);
        ret.Set("files", stat.f_files);
        ret.Set("ffree", stat.f_ffree);
        ret.Set("availableSpace", Number::New(Env(), stat.f_bsize * stat.f_bavail));

        // On FreeBSD statfs struct is not identical
        // field 'fsid' is missing, f_namemax instead of f_namelen for 'nameLen' field.
        #if __FreeBSD__
            ret.Set("nameLen", Number::New(Env(), stat.f_namemax));
        #else
            Array fsid = Array::New(Env(), (size_t) 2);
            for (unsigned i = 0; i<2; i++) {
                fsid[i] = stat.f_fsid.__val[i];
            }
            ret.Set("fsid", fsid);
            ret.Set("nameLen", Number::New(Env(), stat.f_namelen));
        #endif

        Callback().Call({Env().Null(), ret});
    }

};

/**
 * Get FileSystem stats (interface binding)
 */
Value getStatFS(const CallbackInfo& info) {
    Env env = info.Env();

    if (!info[0].IsString()) {
        Error::New(env, "argument fsPath must be a String!").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsFunction()) {
        Error::New(env, "argument callback must be a Function!").ThrowAsJavaScriptException();
        return env.Null();
    }

    string fsPath = info[0].As<String>().Utf8Value();
    Function cb = info[1].As<Function>();
    (new StatFSWorker(cb, fsPath))->Queue();
    
    return env.Undefined();
}

/**
 * Asynchronous Worker
 * @name DiskStatsWorker
 * @desc Retrieve disks stats by reading /proc/diskstats
 * 
 * @header: errno.h
 * @doc: https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats
 */
class DiskStatsWorker : public AsyncWorker {
    public:
        DiskStatsWorker(Function& callback) : AsyncWorker(callback) {}
        ~DiskStatsWorker() {}
    private:
        struct diskstat {
            char* devName;
            unsigned int major;
            unsigned int minor;
            unsigned int ios_pgr;
            unsigned int tot_ticks;
            unsigned int rq_ticks;
            unsigned int wr_ticks;
            unsigned long rd_ios;
            unsigned long rd_merges_or_rd_sec;
            unsigned long rd_ticks_or_wr_sec;
            unsigned long wr_ios;
            unsigned long wr_merges;
            unsigned long rd_sec_or_wr_ios;
            unsigned long wr_sec;
        };

        struct diskbinding {
            string devName;
            int returnedElements;
        };

        vector<diskbinding> entries;

    void Execute() {
        int returnedElements;
        char line[256];

        auto fd = fopen("/proc/diskstats", "r");
        if (fd == NULL) {
            return SetError("failed to open /proc/diskstats");
        }

        diskstat* dstat = (diskstat*) malloc(sizeof(diskstat));
        try {
            while (fgets(line, sizeof(line), fd) != NULL) {
                returnedElements = sscanf(line, "%u %u %s %lu %lu %lu %lu %lu %lu %lu %u %u %u %u",
                    &dstat->major, &dstat->minor, dstat->devName,
                    &dstat->rd_ios, &dstat->rd_merges_or_rd_sec, &dstat->rd_sec_or_wr_ios, &dstat->rd_ticks_or_wr_sec,
                    &dstat->wr_ios, &dstat->wr_merges, &dstat->wr_sec, &dstat->wr_ticks,
                    &dstat->ios_pgr, &dstat->tot_ticks, &dstat->rq_ticks);

                entries.push_back({
                    string(dstat->devName),
                    returnedElements
                });
            }
        }
        catch (std::exception &e) {
            free(dstat);
            fclose(fd);

            return SetError(e.what());
        }

        free(dstat);
        fclose(fd);
    }

    void OnError(const Error& e) {
        stringstream error;
        error << e.what() << ", error code: " << errno << endl;
        Error::New(Env(), error.str()).ThrowAsJavaScriptException();
    }

    void OnOK() {
        HandleScope scope(Env());
        unsigned int j = 0;
        Array ret = Array::New(Env());

        for (size_t i = 0; i < entries.size(); i++) {
            diskbinding stat = entries[i];
            Object JSObject = Object::New(Env());
            ret[j] = JSObject;
    
            if (stat.returnedElements >= 7) {
                JSObject.Set("devName", stat.devName);
                j++;
            }
        }

        Callback().Call({Env().Null(), ret});
    }

};

/**
 * getDiskStats interface binding
 */
Value getDiskStats(const CallbackInfo& info) {
    Env env = info.Env();

    if (!info[0].IsFunction()) {
        Error::New(env, "argument callback must be a Function!").ThrowAsJavaScriptException();
        return env.Null();
    }

    Function cb = info[0].As<Function>();
    (new DiskStatsWorker(cb))->Queue();
    
    return env.Undefined();
}

/*
 * Initialize Node.js Binding exports
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
