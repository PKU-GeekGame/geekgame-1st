FROM python:3

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY backend.py ./
COPY mockers.py ./
COPY build ./build/

CMD ["python", "backend.py"]
EXPOSE 5000
