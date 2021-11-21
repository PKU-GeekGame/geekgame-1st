from flask import Flask, redirect, Response, request, render_template
import utils, base64

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    data = {}
    data['day'] = utils.get_today()
    data['server_addr'] = utils.get_default_server()

    if request.method == 'POST':
        data['server_addr'] = request.form['server']
        data['content'] = request.form['content']
        try:
            base64.b64decode(data['content'].strip(), validate=True)
        except:
            data['content'] = base64.b64encode(data['content'])

        data['response'] = utils.send_msg(data['server_addr'], base64.b64decode(data['content']))
        utils.write_chat_log(data['content'], data['response'])
    
    return render_template('chatbot.html', data=data)

@app.route('/module', methods=['GET'])
def module():
    if 'name' in request.args:
        page = request.args.get('name')
    else:
        page = 'chatlog'
    data = {}
    if 'chatlog' == page:
        try:
            logfile = request.args.get('log') \
                or 'chat_log_%s' % utils.get_today()
            log = open('media/{}'.format(logfile)).read()
        except:
            log = 'No chat log'
        data['log'] = log
    elif 'chatbot' == page:
        data['day'] = utils.get_today()
    else:
        return redirect('/')

    return render_template('{}.html'.format(page), data=data)

@app.route('/src')
def src():
    with open(__file__, encoding='utf-8') as f:
        src = f.read()
    
    resp = Response(src)
    resp.headers['content-type'] = 'text/plain; charset=utf-8'
    return resp

#app.run()