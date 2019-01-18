const Nixfs = require("./");

async function main() {
    const mntEntries = await Nixfs.getMountedEntries();

    for (const ent of mntEntries) {
        console.log("------------");
        console.log(ent.dir);
        try {
            const stat = await Nixfs.getStatFS(ent.dir);
            console.log(stat);
        }
        catch (err) {
            console.log(err.toString());
        }
    }
}
main().catch(console.error);
