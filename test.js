const Nixfs = require("./");

async function main() {
    const stat = await Nixfs.getDiskStats();

    console.log(stat);
}
main().catch(console.error);
