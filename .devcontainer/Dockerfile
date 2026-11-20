FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        freeglut3-dev \
        libglu1-mesa-dev \
        libgl1-mesa-dri \
        libglx-mesa0 \
        xvfb \
        openbox \
        x11vnc \
        novnc \
        websockify \
    && rm -rf /var/lib/apt/lists/*
