/** @type {Nixfs} */
const nixfs = require("node-gyp-build")(__dirname);

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

module.exports = {
    getMountedEntries,
    getStatFS
};
