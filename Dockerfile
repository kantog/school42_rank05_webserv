FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Installeer alleen wat strikt nodig is
RUN apt update && \
    apt install -y --no-install-recommends \
        g++ \
        make \
        python3 \
        php-cgi \
    && apt clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Start met build en run
CMD ["bash", "-c", "make MODE=debug -j && ./webserv users/multipleServers.conf"]
