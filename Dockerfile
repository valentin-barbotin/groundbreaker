FROM ubuntu:22.10

WORKDIR /app

RUN apt-get update -y && apt-get install -y \
    libsdl2-dev libsdl2-2.0-0 \
    libsdl2-gfx-dev libsdl2-gfx-1.0-0 \
    libsdl2-image-dev libsdl2-image-2.0-0 \
    libsdl2-ttf-dev libsdl2-ttf-2.0-0

RUN apt-get install -y make cmake gcc g++ git build-essential

COPY . .

CMD [ "/bin/bash" ]