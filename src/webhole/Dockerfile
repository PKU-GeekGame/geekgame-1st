FROM python:3

# install libs

RUN apt-get update && apt-get install -y \
    fonts-liberation libappindicator3-1 libasound2 libatk-bridge2.0-0 \
    libnspr4 libnss3 lsb-release xdg-utils libxss1 libdbus-glib-1-2 \
    unzip xvfb

# install chrome

COPY game/xssbot/vendor/google-chrome-stable_current_amd64.deb /usr/src/
RUN apt-get install /usr/src/google-chrome-stable_current_amd64.deb -y -f && \
    rm /usr/src/google-chrome-stable_current_amd64.deb

# copy chrome driver

COPY game/xssbot/vendor/chromedriver /usr/bin/
RUN chmod +x /usr/bin/chromedriver

# install python requirements

WORKDIR /usr/src/app

COPY game/xssbot/requirements.txt ./
RUN pip3 install -r requirements.txt

# copy src

COPY game/xssbot/flag.py ./
COPY game/xssbot/logger.py ./
COPY game/xssbot/xssbot.py ./
COPY game/xssbot/cert.pem ./

CMD ["python", "xssbot.py"]