FROM python:3

WORKDIR /usr/src/app

COPY game ./

RUN pip install --no-cache-dir -r requirements.txt
RUN mkdir -p db

CMD ["python", "app.py"]
EXPOSE 5000
