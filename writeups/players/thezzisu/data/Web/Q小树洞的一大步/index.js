const http = require("http");
const fs = require("fs");
const path = require("path");

const server = http.createServer((req, res) => {
  console.log(req.url);
  console.log(req.headers);
  res.setHeader("Access-Control-Allow-Origin", "*");
  res.setHeader("Access-Control-Request-Method", "*");
  res.setHeader("Access-Control-Allow-Methods", "OPTIONS, GET");
  res.setHeader("Access-Control-Allow-Headers", "*");
  res.setHeader("Set-Cookie", "mycookie=test");
  if (req.method === "OPTIONS") {
    res.writeHead(200);
    res.end();
    return;
  }
  if (/\.\./.test(req.url)) {
    res.end("Civilized words.");
    return;
  }
  if (req.url.endsWith(".js")) {
    res.setHeader("Content-Type", "application/javascript");
  }
  const file = path.resolve(__dirname, req.url.substr(1));
  if (req.url.substr(1) && fs.existsSync(file)) {
    fs.createReadStream(file).pipe(res);
  } else {
    res.end("Civilized words.");
  }
});

server.listen(1926);
