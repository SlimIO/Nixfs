/** @type {Nixfs} */
const nixfs = require("node-gyp-build")(__dirname);

const nfs = nixfs.getMountedEntries();
for (const fs of nfs) {
    console.log(nixfs.getStatFS(fs.dir));
}
