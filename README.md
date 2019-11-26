# Nixfs
![V1.0](https://img.shields.io/badge/version-1.0.0-blue.svg)
![N-API](https://img.shields.io/badge/N--API-v3-green.svg)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/SlimIO/Nixfs/commit-activity)
[![GitHub license](https://img.shields.io/github/license/Naereen/StrapDown.js.svg)](https://github.com/SlimIO/Nixmem/blob/master/LICENSE)
![2DEP](https://img.shields.io/badge/Dependencies-2-yellow.svg)
[![Build Status](https://travis-ci.com/SlimIO/Nixfs.svg?branch=master)](https://travis-ci.com/SlimIO/Nixfs) [![Greenkeeper badge](https://badges.greenkeeper.io/SlimIO/Nixfs.svg)](https://greenkeeper.io/)

SlimIO Nixfs is a Node.js binding which bring information about filesystem configuration and consumption. This module has been designed for UNIX systems.

## OS Support

| Linux | FreeBSD | OpenBSD | Sun | Mac (OS X) |
| --- | --- | --- | --- | --- |
| ✔️ | ⚠️ | ⚠️ | ❌ | ❌ |

> ⚠️ WORK IN PROGRESS

## Requirements
- [Node.js](https://nodejs.org/en/) v10 or higher

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

<details><summary>getMountedEntries(): Promise< mountentry[] ></summary>
<br />

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
</details>

<details><summary>getStatFS(dir: string): Promise< fsstat ></summary>
<br />

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
</details>


<details><summary>getDiskStats(): Promise< diskstat[] ></summary>
<br />

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
</details>

## Contribution Guidelines
To contribute to the project, please read the [code of conduct](https://github.com/SlimIO/Governance/blob/master/COC_POLICY.md) and the guide for [N-API compilation](https://github.com/SlimIO/Governance/blob/master/docs/native_addons.md).

## Dependencies

|Name|Refactoring|Security Risk|Usage|
|---|---|---|---|
|[node-addon-api](https://github.com/nodejs/node-addon-api)|⚠️Major|Low|Node.js C++ addon api|
|[node-gyp-build](https://github.com/prebuild/node-gyp-build)|⚠️Major|Low|Node-gyp builder|

## License
MIT
