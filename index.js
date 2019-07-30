"use strict";

/**
 * @namespace Nixfs
 */

/** @type {Nixfs} */
const nixfs = require("node-gyp-build")(__dirname);

/**
 * @function getMountedEntries
 * @exports Nixfs/getMountedEntries
 * @returns {Promise<Nixfs.mountentry[]>}
 */
function getMountedEntries() {
    return new Promise((resolve, reject) => {
        nixfs.getMountedEntries((err, entries) => {
            if (err) {
                return reject(err);
            }

            return resolve(entries);
        });
    });
}

/**
 * @function getStatFS
 * @exports Nixfs/getStatFS
 * @param {!string} dir dir
 * @returns {Promise<Nixfs.fsstat>}
 */
function getStatFS(dir) {
    return new Promise((resolve, reject) => {
        nixfs.getStatFS(dir, (err, stat) => {
            if (err) {
                return reject(err);
            }

            return resolve(stat);
        });
    });
}

/**
 * @function getDiskStats
 * @exports Nixfs/getDiskStats
 * @returns {Promise<Nixfs.diskstat[]>}
 */
function getDiskStats() {
    return new Promise((resolve, reject) => {
        nixfs.getDiskStats((err, stat) => {
            if (err) {
                return reject(err);
            }

            return resolve(stat);
        });
    });
}

module.exports = {
    getMountedEntries,
    getStatFS,
    getDiskStats
};
