import os
import json
from shutil import copyfile
from flask import Flask,request,render_template,url_for,send_from_directory,make_response,redirect
from werkzeug.middleware.proxy_fix import ProxyFix
from flask import jsonify
from hashlib import md5
import signal
from http.server import HTTPServer, SimpleHTTPRequestHandler

os.environ['TEMP']='/dev/shm'

app = Flask("access")
app.wsgi_app = ProxyFix(app.wsgi_app, x_for=1 ,x_proto=1)


@app.route('/',methods=['POST', 'GET'])
def index():
    if request.method == 'POST':
        f=request.files['file']
        os.system("rm -rf /dev/shm/zip/media/*")
        path=os.path.join("/dev/shm/zip/media",'tmp.zip')
        f.save(path)
        os.system('timeout -k 1 3 unzip /dev/shm/zip/media/tmp.zip -d /dev/shm/zip/media/')
        os.system('rm /dev/shm/zip/media/tmp.zip')
        return redirect('/media/')
    response = render_template('index.html')
    return response

@app.route('/media/',methods=['GET'])
@app.route('/media',methods=['GET'])
@app.route('/media/<path>',methods=['GET'])
def media(path=""):
    npath=os.path.join("/dev/shm/zip/media",path)
    print("111")
    print(npath)
    if not os.path.exists(npath):
        return make_response("404",404)
    if not os.path.isdir(npath):
        f=open(npath,'rb')
        response = make_response(f.read())
        response.headers['Content-Type'] = 'application/octet-stream'
        return response
    else:
        fn=os.listdir(npath)
        fn=[".."]+fn
        f=open("templates/template.html")
        x=f.read()
        f.close()
        ret="<h1>文件列表:</h1><br><hr>"
        for i in fn:
            tpath=os.path.join('/media/',path,i)
            ret+="<a href='"+tpath+"'>"+i+"</a><br>"
        x=x.replace("HTMLTEXT",ret)
        return x


os.system('mkdir /dev/shm/zip')
os.system('mkdir /dev/shm/zip/media')

app.run(host="0.0.0.0",port=8080,debug=False,threaded=True)
