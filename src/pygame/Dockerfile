FROM python:3

WORKDIR /usr/src/app

COPY game/server ./

RUN pip install --no-cache-dir -r requirements.txt
RUN mkdir -p logs

CMD ["python", "server.py"]
EXPOSE 10010
