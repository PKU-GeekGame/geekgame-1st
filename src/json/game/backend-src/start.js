if(process.platform!=='linux')
    process.exit(1);

const server = require('jsonaas-backend');
server(5000);