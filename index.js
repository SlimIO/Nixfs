/** @type {Nixfs} */
const nixfs = require("bindings")("nixfs");

const nfs = nixfs.getMountedEntries();
for (const fs of nfs) {
    console.log(nixfs.getStatFS(fs.dir));
}
