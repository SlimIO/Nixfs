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
        type: number;
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

    export function getMountedEntries(): mountentry[];
    export function getStatFS(dir: string): fsstat;
}

export as namespace Nixfs;
export = Nixfs;
