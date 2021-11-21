FROM openjdk:17-jdk-slim

COPY . /var/www/geek_dragon
WORKDIR /var/www/geek_dragon

RUN chmod +x ./gradlew
RUN ./gradlew clean projectZip build
