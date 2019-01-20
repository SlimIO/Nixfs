# Nixfs
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/SlimIO/Nixfs/commit-activity)
[![GitHub license](https://img.shields.io/github/license/Naereen/StrapDown.js.svg)](https://github.com/SlimIO/Nixmem/blob/master/LICENSE)
![V1.0](https://img.shields.io/badge/version-1.0.0-blue.svg)
![N-API](https://img.shields.io/badge/N--API-experimental-orange.svg)

SlimIO Nixfs is a Node.js binding which bring information about filesystem configuration and consumption. This module has been designed for UNIX systems.

| Linux | FreeBSD | OpenBSD | Sun | Mac (OS X) |
| --- | --- | --- | --- | --- |
| ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |

## Getting Started

This package is available in the Node Package Repository and can be easily installed with [npm](https://docs.npmjs.com/getting-started/what-is-npm) or [yarn](https://yarnpkg.com).

```bash
$ npm i @slimio/nixfs
# or
$ yarn add @slimio/nixfs
```

## Usage example

```js
const Nixfs = require("@slimio/nixfs"); 

async function main() {
    const mntEntries = await Nixfs.getMountedEntries();
    for (const ent of mntEntries) {
        console.log(`Entry name: ${ent.name}`);
        const fileSystemStat = await Nixfs.getStatFS(ent.dir);
        console.log(fileSystemStat);
        console.log("-----------------\n");
    }
}
main().catch(console.error);
```

## API

### getMountedEntries(): Promise< mountentry[] >
Retrieve mounted entries on the local system. Return an array of `mountentry` object. On FreeBSD the data is retrieved by reading the `/etc/fstab` file.

```ts
interface mountentry {
    dir: string;
    name: string;
    type: string;
    freq: number;
    passno: number;
    options: string[];
}
```

### getStatFS(dir: string): Promise< fsstat >
Retrieve statistic for a given file system directory. Return an `fsstat` Object. It use UNIX [statfs](http://www.tutorialspoint.com/unix_system_calls/statfs.htm) under the hood.

```ts
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
```

> Note: fsid is not available on freebsd!

### getDiskStats(): Promise< diskstat[] >
Retrieve all Disks statistics. Return an array of `diskstat` object. The data is retrieved by reading the `/proc/diskstats` file (a documentation describing this file can be found [here](https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats))

```ts
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
```

## Developer Guide
If you want to contribute but don't know how to compile etc.. Take a look at this [documentation](./BUILD.md)

## Licence
MIT
