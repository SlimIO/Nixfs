// Require Third-party Dependencies
const ava = require("ava");
const is = require("@slimio/is");

// Require Internal Dependencies
const Nixfs = require("../");

ava("check exported methods", (assert) => {
    assert.deepEqual(Object.keys(Nixfs), ["getMountedEntries", "getStatFS", "getDiskStats"]);
});

ava("retrieve mounted entries", async(assert) => {
    const mntEntries = await Nixfs.getMountedEntries();
    assert.true(is.array(mntEntries));
    for (const ent of mntEntries) {
        assert.true(is.plainObject(ent));
        assert.true(is.string(ent.dir));
        assert.true(is.string(ent.name));
        assert.true(is.string(ent.type));
        assert.true(is.string(ent.options));
        assert.true(is.number(ent.freq));
        assert.true(is.number(ent.passno));
    }
});

ava("retrieve filesystem stat", async(assert) => {
    const mntEntries = await Nixfs.getMountedEntries();
    for (const ent of mntEntries) {
        const stat = await Nixfs.getStatFS(ent.dir);
        assert.true(is.plainObject(stat));

        assert.true(is.string(stat.type));
        assert.true(is.number(stat.typeId));
        assert.true(is.number(stat.bsize));
        assert.true(is.number(stat.blocks));
        assert.true(is.number(stat.bfree));
        assert.true(is.number(stat.bavail));
        assert.true(is.number(stat.files));
        assert.true(is.number(stat.ffree));
        assert.true(is.number(stat.availableSpace));
        assert.true(is.number(stat.nameLen));
        if (process.platform !== "freebsd") {
            assert.true(is.array(stat.fsid));
            assert.true(is.number(stat.fsid[0]));
            assert.true(is.number(stat.fsid[1]));
        }
    }
});
