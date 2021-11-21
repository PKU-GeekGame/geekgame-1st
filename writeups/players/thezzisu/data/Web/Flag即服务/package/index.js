const express = require('express');
const session = require('express-session');
const helmet = require('helmet');
const fs = require('fs');

const {getflag} = require('./getflag');
const {safe_eval} = require('./safe_eval');

let FLAG0 = getflag('flag0.txt');
let FLAG1 = getflag('flag1.txt');
let FLAG2 = getflag('flag2.txt');
const MAX_LENGTH = 4096;

const app = express();
app.use(helmet());
app.use(session({
    secret: 'ctl7nk2s170srnivd4r7vj9rh5dv4dgi',
    resave: false,
    saveUninitialized: false,
}));

app.use((req, res, next)=>{
    if(req.path.toLowerCase().endsWith('settings'))
        res.send('this feature is currently under development :(');
    else
        next();
});

app.get('/', (req, res)=>{
    res.sendFile('readme.html', {root: '.'});
});

function waf(str) {
    for(let bad_name of Object.getOwnPropertyNames(({}).__proto__))
        if(str.indexOf(bad_name)!==-1)
            return true;
    return false;
}

app.get('/api/:path(*)', (req, res)=>{
    let path = 'data/'+req.params.path;
    let in_path = req.query.in_path||'';
    let out_path = req.query.out_path||'';
    let prefix = req.session.prefix ? (req.session.prefix+'/') : '';
    let eval_mode = req.session.eval_enabled===1;

    if(waf(in_path) || waf(out_path) || waf(prefix)) {
        res.send('Bad parameter!');
        return;
    }

    if(!fs.existsSync(path)) {
        res.send('File does not exists!');
        return;
    }
    let data = fs.readFileSync(path);
    if(data.length>MAX_LENGTH) {
        res.send('File too big!');
        return;
    }
    try {
        data = JSON.parse(data);
    } catch(_) {
        res.send('Cannot parse file as JSON!');
        return;
    }

    in_path = prefix + in_path;
    in_path = in_path.split('/').filter(x=>x!=='');
    for(let term of in_path) {
        if(term.indexOf('_')!==-1) {
            res.send('Bad parameter!');
            return;
        }
        if(eval_mode && /^\([^a-zA-Z"',;]+\)$/.test(term))
            term = safe_eval(term);
        if(data[term]===undefined)
            data[term] = {};
        data = data[term];
    }

    if(!JSON.stringify(data)) {
        res.send('Bad data!');
        return;
    }

    let output = {};
    out_path = prefix + out_path;
    out_path = out_path.split('/').filter(x=>x!=='');
    if(out_path.length===0)
        output = data;
    else {
        let cur = output;
        for(let term of out_path.slice(0, out_path.length-1)) {
            if(term.indexOf('_')!==-1) {
                res.send('Bad parameter!');
                return;
            }
            // no eval for out_path :)
            /*
            if(eval_mode && /^\([^a-zA-Z"',;]+\)$/.test(term))
                term = safe_eval(term);
            */
            if(cur[term]===undefined)
                cur[term] = {};
            cur = cur[term];
        }
        cur[out_path[out_path.length-1]] = data;
    }

    res.json(output);
});

app.get('/activate', (req, res)=>{
    if(req.query.code===FLAG1)
        req.session.activated = 1;
    
    if(req.session.activated)
        res.send(`You have been activated. Activation code: ${FLAG1}`);
    else
        res.send('Wrong activation code :(');
});

app.get('/eval_settings', (req, res)=>{
    if(req.session.activated!==1) {
        res.send('Eval feature requires activation :(');
        return;
    }

    req.session.eval_enabled = req.query.eval==='on' ? 1 : 0;
    res.send(`Eval set to ${req.session.eval_enabled}`);
});

app.get('/prefix_settings', (req, res)=>{
    if(req.session.activated!==1) {
        res.send('Prefix feature requires activation :(');
        return;
    }
    
    if(waf(req.query.prefix)) {
        res.send('Bad prefix!');
        return;
    }

    req.session.prefix = req.query.prefix;
    res.send(`Prefix set to "${req.session.prefix}"`);
});

module.exports = function start_server(port) {
    if(FLAG0!==`flag{${0.1+0.2}}`)
        return;
    FLAG2 = null;
    app.listen(port, ()=>{
        console.log(`server started on :${port}`);
    });
}