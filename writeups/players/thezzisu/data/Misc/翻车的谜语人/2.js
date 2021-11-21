const cp = require("child_process");
const { readFileSync, writeFileSync } = require("fs");
const util = require("util");
const exec = util.promisify(cp.exec);

/**
 * @param {string} pwd
 */
async function check(pwd) {
  try {
    await exec(`7za t flag2.7z -p"${pwd}"`);
    return true;
  } catch {
    return false;
  }
}

async function main() {
  /** @type {string[]} */
  const tz = "Sat 06 Nov 2021 03:44:14 PM CST";
  for (let nproc = 8; nproc >= 1; nproc--) {
    console.log(`Try ${tz}`);
    // Sat Nov  6 07:44:18 GMT 2021
    for (let sec = 20; sec >= 10; sec--) {
      const time =
        tz.substr(0, 22) + ("" + sec).padStart(2, "0") + tz.substr(24);
      // Wakarimasu! `date` `uname -nom` `nproc`
      const pwd = `Wakarimasu! ${time} you-kali-vm x86_64 GNU/Linux ${nproc}`;
      console.log(`Using "${pwd}"`);
      if (await check(pwd)) {
        console.log(pwd);
        writeFileSync("pwd.txt", pwd);
        process.exit(0);
      }
    }
  }
}

main();
