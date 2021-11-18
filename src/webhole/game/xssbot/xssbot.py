from selenium import webdriver
from pyvirtualdisplay import Display
import time
import sys
import traceback

import logger
from flag import getflag, checktoken

import functools
print = functools.partial(print, flush=True)

display = Display(visible=0, size=(800, 600))
display.start()
time.sleep(.3)

#HOLE_URL = 'http://127.0.0.1:8000/hole/'
HOLE_URL = 'https://prob15-qkuhole.geekgame.pku.edu.cn/hole/'

token = input('Token: ')
uid = checktoken(token)

if not uid:
    print('bad token!')
    sys.exit(1)

FLAG = getflag(token, 0)

print('show me the url of your blog, and i will visit it later.')
url = input('> ')

logger.write(uid, ['enter_url', url])

print('- starting up my browser...')

def get_traceback(e):
    lines = traceback.format_exception(type(e), e, e.__traceback__)
    return ''.join(lines)

try:
    options = webdriver.ChromeOptions()
    options.add_argument('--no-sandbox')
    options.add_experimental_option('excludeSwitches', ['enable-logging'])

    with webdriver.Chrome(options=options) as driver:
        ua = driver.execute_script('return navigator.userAgent')
        print(' i am using', ua)
        
        print('- browsing qku hole...')
        driver.get(HOLE_URL)
        time.sleep(4)
        
        print(' putting secret flag...')
        driver.execute_script(f'document.cookie="flag={FLAG}"')
        time.sleep(1)
        
        title = driver.title
        #print(' the page says:', title)
        logger.write(uid, ['title_1', title])
        time.sleep(1)
        
        print('- browsing another site...')
        driver.get('https://example.com')
        time.sleep(2)
        
        title = driver.title
        #print(' the page says:', title)
        logger.write(uid, ['title_2', title])
        time.sleep(1)
        
        print('- now browsing your blog...')
        driver.get(url)
        logger.write(uid, ['source_3', driver.page_source])
        time.sleep(6)
        
        title = driver.title
        #print(' the page says:', title)
        logger.write(uid, ['title_3', title])
        time.sleep(1)
        
        print(' your blog looks great!')
    
    print('- see you later :)')
    logger.write(uid, ['leaving'])
    
except Exception as e:
    print('ERROR', type(e))
    
    tb = get_traceback(e)
    logger.write(uid, ['exception', str(type(e)), str(e), tb])