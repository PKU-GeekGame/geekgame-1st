const fs = require('fs');
fs.writeFileSync('flag1.txt', 'fake{flag1}');
fs.writeFileSync('flag2.txt', 'fake{flag2}');

if(process.platform!=='linux')
    process.exit(1);

const server = require('jsonaas-backend');
server(3001);