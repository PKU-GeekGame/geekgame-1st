const fs = require("fs");

const dump = fs.readFileSync("out.txt").toString();

console.log(
  dump
    .split("\n")
    .map((x) => x.substr(x.indexOf("[")))
    .map((x) => JSON.parse(x))
    .map((x) => x[1])
    .join("")
);
