FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && \
    apt install -y --no-install-recommends \
        g++ \
        make \
        python3 \
        php-cgi \
    && apt clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

CMD ["bash", "-c", "make MODE=debug -j && ./webserv users/multipleServers.conf"]

# docker build -t mijnwebserv .
# docker run -it --rm -p 8000:8000 -p 8080:8080 -v ${PWD}:/app mijnwebserv