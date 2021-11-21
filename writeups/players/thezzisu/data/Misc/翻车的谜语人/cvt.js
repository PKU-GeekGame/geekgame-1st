const fs = require("fs");

const dump = JSON.parse(fs.readFileSync("dump1.json").toString());

console.log(
  dump.content.cells
    .map((cell) => cell.source)
    .filter((x) => x)
    .join("\n")
);
