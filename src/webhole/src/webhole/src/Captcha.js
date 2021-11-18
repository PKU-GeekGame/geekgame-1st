import React, {createContext, useState, useContext, useRef, useEffect} from 'react';
import {createPortal} from 'react-dom';
import {GoogleReCaptchaProvider, useGoogleReCaptcha} from 'react-google-recaptcha-v3';

import './Captcha.css';
import {QKUHELPER_ROOT, token_param, get_json} from './flows_api';
import {TokenCtx} from './UserAction';

const KEY_MAX=100000000;
const RECAPTCHA_KEY='6LeV_9MZAAAAAAwKy_IxuozNJ-96sbBIujsqUSXn';

let CaptchaCtx=createContext({
    get_key: ()=>-1,
    add_listener: (k,fn)=>{},
    remove_listener: (k)=>{},
});


function CaptchaSelf(props) {
    let [portal_dom,_]=useState(()=>document.createElement('div'));
    let {executeRecaptcha: exe_recaptcha}=useGoogleReCaptcha();
    let {value: token}=useContext(TokenCtx);
    let [hint_msg,set_hint_msg]=useState('请完成人机验证');

    useEffect(()=>{
        document.body.appendChild(portal_dom);
        return ()=>{
            portal_dom.parentNode.removeChild(portal_dom);
        };
    },[]);

    function run_captcha() {
        set_hint_msg('正在进行人机验证');
        console.log('captcha: exec');
        exe_recaptcha('hole')
            . then((captcha_result)=>{
                set_hint_msg('正在检查人机验证');
                console.log('captcha: submit');

                let data=new URLSearchParams();
                data.append('recaptcha',captcha_result);
                return fetch(QKUHELPER_ROOT+'services/qkuhole/api.php?action=captcha'+token_param(token), {
                    method: 'post',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: data,
                })
            })
            .then(get_json)
            .then((json)=>{
                if(json.code!==0)
                    throw new Error(json.msg||JSON.stringify(json));

                set_hint_msg('人机验证完成');
            })
            .then(()=>{
                props.onFinish();
            })
            .catch((e)=>{
                alert('人机验证失败：'+e);
                set_hint_msg('人机验证失败');
            });
    }

    useEffect(()=>{
        run_captcha();
    },[]);

    return createPortal(
        <div className="captcha-self">
            <div className="box box-tip">
                {hint_msg}
                （<a onClick={run_captcha}>重试</a>）
            </div>
        </div>,
        portal_dom,
    );
}

export function CaptchaRoot(props) {
    let listeners=useRef([]);
    let [should_show_self, set_should_show_self]=useState(false);
    let key=useRef(0);

    function captcha_done() {
        let l=listeners.current.slice();
        listeners.current=[];
        set_should_show_self(false);
        l.forEach(([k,fn])=>{
            console.log('captcha: callback',k);
            fn();
        });
    }

    return (
        <CaptchaCtx.Provider value={{
            get_key: ()=>{
                if((++key.current)>=KEY_MAX)
                    key.current%=KEY_MAX;
                return key.current;
            },
            add_listener: (k,fn)=>{
                if(k===null) return;
                if(listeners.current.some(([ik,_ifn])=>ik===k))
                    return;
                console.log('captcha: add',k);
                listeners.current.push([k,fn]);
                set_should_show_self(true);
            },
            remove_listener: (k)=>{
                console.log('captcha: remove',k);
                listeners.current=listeners.current.filter(([ik,_ifn])=>ik!==k);
                if(listeners.current.length===0)
                    set_should_show_self(false);
            },
        }}>
            <GoogleReCaptchaProvider reCaptchaKey={RECAPTCHA_KEY} language="zh-CN" useRecaptchaNet={true}>
                {props.children}
                {should_show_self &&
                    <CaptchaSelf onFinish={captcha_done} />
                }
            </GoogleReCaptchaProvider>
        </CaptchaCtx.Provider>
    )
}

export function CaptchaClient(props) {
    let ctx=useContext(CaptchaCtx);
    let k=useRef(null);

    useEffect(()=>{
        k.current=ctx.get_key();
        ctx.add_listener(k.current,props.onFinish);
        return ()=>{
            ctx.remove_listener(k.current);
        }
    },[]);

    return (
        <p>
            <a onClick={()=>{
                ctx.add_listener(k.current,props.onFinish);
            }}>进行人机验证</a>
            （<a onClick={props.onFinish}>完成</a>）
        </p>
    );
}