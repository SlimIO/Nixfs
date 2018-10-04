declare namespace Nixfs {

    interface mountentry {
        dir: string;
        name: string;
        type: string;
        freq: number;
        passno: number;
        opts: string;
    }

    interface fsstat {
        type: string;
        typeId: number;
        bsize: number;
        blocks: number;
        bfree: number;
        bavail: number;
        files: number;
        ffree: number;
        availableSpace: number;
        fsid: [number, number];
        nameLen: number;
    }

    interface diskstat {
        devName: string;
        major: number;
        minor: number;
        rdIos: number;
        rdMergesOrRdSec: number;
        rdSecOrWrIos: number;
        rdTicksOrWrSec: number;
        wrIos: number;
        wrMerges: number;
        wrSec: number;
        wrTicks: number;
        iosPgr: number;
        totTicks: number;
        rqTicks: number;
    }

    export function getMountedEntries(): mountentry[];
    export function getStatFS(dir: string): fsstat;
    export function getDiskStats(): diskstat[];
}

export as namespace Nixfs;
export = Nixfs;
