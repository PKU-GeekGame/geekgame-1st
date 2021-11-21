from selenium import webdriver
import time

HOLE_URL = 'https://prob15-qkuhole.geekgame.pku.edu.cn/hole/'
FLAG = 'fake{get the real flag on the server}'

print('show me the url of your blog, and i will visit it later.')
url = input('> ')

print('- starting up my browser...')

try:
    options = webdriver.ChromeOptions()
    options.add_argument('--no-sandbox') # sandbox not working in docker
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
        
        #print(' the page says:', driver.title)
        time.sleep(1)
        
        print('- browsing another site...')
        driver.get('https://example.com')
        time.sleep(2)
        
        #print(' the page says:', driver.title)
        time.sleep(1)
        
        print('- now browsing your blog...')
        driver.get(url)
        time.sleep(6)
        
        #print(' the page says:', driver.title)
        time.sleep(1)
        
        print(' your blog looks great!')
        
    print('- see you later :)')
    
except Exception as e:
    print('ERROR', type(e))