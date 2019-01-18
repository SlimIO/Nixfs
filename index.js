/** @type {Nixfs} */
const nixfs = require("node-gyp-build")(__dirname);

nixfs.getMountedEntries((err, ret) => {
    if (err) {
        console.error(err);
    }

    console.log(ret);
});
