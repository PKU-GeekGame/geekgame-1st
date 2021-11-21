const data = `fa{aeAGetTm@ekaev!
lgHv__ra_ieGeGm_1}`;
let [str1, str2] = data.split("\n");
console.log([...str1].map((x, i) => x + str2[i]).join(""));
