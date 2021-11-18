FROM python:3.7-alpine

LABEL maintainer="wofeiwo <wofeiwo@gmail.com>"

ADD requirements.txt /tmp/requirements.txt
ADD flag /flag
ADD files/config/uwsgi-ctf.ini /tmp/uwsgi-ctf.ini
ADD files/config/supervisor-ctf.conf /etc/supervisor-ctf.conf

COPY ./src/ /usr/src/
WORKDIR /usr/src/ufctf

RUN set -ex \
    && apk add --no-cache --virtual .build-deps build-base curl-dev linux-headers \
    && pip install -r /tmp/requirements.txt

RUN set -ex \
    && chown root:root -R . \
    && chmod 0666 -R /tmp/* \
    && chown root:root /flag \
    && chmod 0600 /flag

CMD ["sh", "run.sh"]