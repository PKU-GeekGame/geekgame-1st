const fs = require('fs');
const crypto = require('crypto');

function getflag(path) {
    let f;
    try {
        f = fs.openSync(path);
    } catch(e) {
        //return 'failed';
        throw e;
    }
    let content = fs.readFileSync(f, {encoding: 'utf-8'}).trim();
    fs.unlinkSync(path);
    return content;
}

module.exports = {
    getflag: getflag,
};