#!/bin/sh

# docker stop ctf && docker rm ctf && docker rmi ufctf
docker-compose down && docker rmi ufctf_web
