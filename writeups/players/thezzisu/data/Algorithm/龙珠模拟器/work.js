// @ts-check
import fetch from "node-fetch";
import fs from "fs";

const TOKEN = "TOKEN";
const COOKIE = "COOKIE";

/**
 * @template T
 * @param {() => Promise<T>} fn
 * @param {number} max
 * @returns {Promise<T>}
 */
async function retry(fn, max = 5) {
  for (let i = 0; i < max; i++) {
    try {
      return await fn();
    } catch {
      console.log("retry...");
    }
  }
}

/**
 * @returns {Promise<void>}
 */
async function reset() {
  await retry(() =>
    fetch(
      `https://prob16.geekgame.pku.edu.cn/api/reset?token=${TOKEN}&super=false`,
      {
        headers: {
          accept: "*/*",
          cookie: COOKIE,
        },
        referrer: "https://prob16.geekgame.pku.edu.cn/",
        referrerPolicy: "strict-origin-when-cross-origin",
        body: null,
        method: "POST",
      }
    )
  );
}

/**
 * @param {number} x
 * @param {number} y
 * @typedef {{
 *   min: [number, number],
 *   materials: string[][],
 *   expiration: string
 *   flag?: string
 * }} IState
 * @returns {Promise<IState>}
 */
async function state(x, y) {
  const res = await retry(() =>
    fetch(
      `https://prob16.geekgame.pku.edu.cn/api/state?token=${TOKEN}&x=${x}&y=${y}`,
      {
        headers: {
          accept: "*/*",
          cookie: COOKIE,
        },
        referrer: "https://prob16.geekgame.pku.edu.cn/",
        referrerPolicy: "strict-origin-when-cross-origin",
        body: null,
        method: "GET",
      }
    )
  );
  /** @type {any} */
  const data = await res.json();
  if (data.flag) {
    console.log(data.flag);
    fs.writeFileSync("./flag.txt", data.flag);
    process.exit(0);
  }
  return data;
}

/**
 * @param {number} x
 * @param {number} y
 * @returns {Promise<{
 *   balls: {
 *     [key: string]: number
 *   }
 * }>}
 */
async function submit(x, y) {
  const res = await retry(() =>
    fetch(
      `https://prob16.geekgame.pku.edu.cn/api/break?token=${TOKEN}&x=${x}&y=${y}`,
      {
        headers: {
          accept: "*/*",
          cookie: COOKIE,
        },
        referrer: "https://prob16.geekgame.pku.edu.cn/",
        referrerPolicy: "strict-origin-when-cross-origin",
        body: null,
        method: "POST",
      }
    )
  );
  /** @type {any} */
  const data = await res.json();
  return data;
}

/**
 * @param {IState} s
 * @typedef {{ [key: string]: [number, number][] }} BallDict
 * @returns {BallDict}
 */
function convert(s) {
  /** @type {BallDict} */
  const map = Object.create(null);
  s.materials.forEach((t, i) =>
    t.forEach((x, j) => (map[x] = map[x] ?? []).push([i, j]))
  );
  delete map["AIR"];
  return map;
}

/**
 * @param {number} x
 * @param {number} y
 * @returns {Promise<string>}
 */
async function getType(x, y) {
  const data = await state(x, y);
  return data.materials[x - data.min[0]][y - data.min[1]];
}

const data = [24, 48, 192, 1536, 24576, 393216, 25165824];
const name = [
  "FIRST",
  "SECOND",
  "THIRD",
  "FOURTH",
  "FIFTH",
  "SIXTH",
  "SEVENTH",
];

/**
 * @param {number} x
 * @param {number} y
 * @param {number} z
 * @returns {Promise<[number, number]>}
 */
async function calcNext(x, y, z) {
  const d = data[z] / data[z - 1];
  const w = data[z - 1];
  console.log(`Finding Ball ${name[z]}, d = ${d}, w = ${w}`);
  for (let i = 0; i < d; i++) {
    for (let j = 0; j < d; j++) {
      const t = await getType(i * w + x, j * w + y);
      if (t !== "AIR") console.log(`${i} ${j} => ${t}`);
      if (t === name[z]) {
        return [i * w + x, j * w + y];
      }
    }
  }
  return [x, y];
}

async function main() {
  await reset();
  const raw = await state(0, 0);
  const s0 = convert(raw);
  console.log(s0);
  const ans = [s0["FIRST"][0]];
  for (let i = 1; i < 7; i++) {
    ans.push(await calcNext(...ans[i - 1], i));
  }
  for (let i = 0; i < 7; i++) {
    await submit(...ans[i]);
  }
  await state(0, 0);
}

main();
