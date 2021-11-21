const child_process = require('child_process');

const CLIENT_CODE = `
const vm = require("vm");
const code = __USERCODE__;
console.log(vm.runInNewContext(code, {}));
`;
const TIMEOUT_MS = 1000;

function safe_eval(s) {
    let code = CLIENT_CODE.replace('__USERCODE__', JSON.stringify(s));
    try {
        let stdout = child_process.execFileSync('/usr/local/bin/node', ['-'], {
            input: code,
            env: {},
            timeout: TIMEOUT_MS,
            encoding: 'utf-8',
            killSignal: 'SIGTERM',
        });
        return stdout.trim();
    } catch(_) {
        return s;
    }
}

module.exports = {
    safe_eval: safe_eval,
};