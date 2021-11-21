const fs = require("fs");
const cp = require("child_process");

const left = fs.readFileSync("left.bin");
const right = fs.readFileSync("right.bin");

fs.writeFileSync("left.tmp", [...left].map((v) => `${v}`).join("\n"));
fs.writeFileSync("right.tmp", [...right].map((v) => `${v}`).join("\n"));
try {
  cp.execSync("diff -d left.tmp right.tmp > diff.tmp");
} catch {}
const diff = fs.readFileSync("diff.tmp");
const buf = Buffer.from(
  diff
    .toString()
    .split("\n")
    .map((x) => x.trim())
    .filter((x) => x && "><".includes(x[0]))
    .map((x) => parseInt(x.substr(2)))
);
fs.writeFileSync("diff.bin", buf);
