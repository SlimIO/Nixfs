declare namespace Nixfs {

    interface mountentry {
        dir: string;
        name: string;
        type: string;
        freq: number;
        passno: number;
        options: string;
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
        fsid?: [number, number];
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

    export function getMountedEntries(): Promise<mountentry[]>;
    export function getStatFS(dir: string): Promise<fsstat>;
    export function getDiskStats(): Promise<diskstat[]>;
}

export as namespace Nixfs;
export = Nixfs;
