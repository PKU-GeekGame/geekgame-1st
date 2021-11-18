FROM node:14

WORKDIR /usr/src/app
COPY game/backend-src .
COPY game/data/demo.json data/

RUN npm install

EXPOSE 3001
CMD [ "npm", "start" ]