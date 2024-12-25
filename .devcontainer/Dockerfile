FROM ubuntu:24.04

# Update and install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libgl1-mesa-dev \
    libasound2-dev \
    libpulse-dev \
    libudev-dev \
    libx11-dev \
    libxext-dev \
    libxi-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxinerama-dev \
    libxi-dev \
    libxss-dev \
    libxxf86vm-dev \
    wget \
    tar

# Install Emscripten for WebAssembly
RUN git clone https://github.com/emscripten-core/emsdk.git /emsdk \
    && cd /emsdk \
    && ./emsdk install latest \
    && ./emsdk activate latest

# Environment setup for Emscripten
ENV PATH="/emsdk:/emsdk/upstream/emscripten:${PATH}"
ENV EMSDK="/emsdk"

# Download and compile SDL3 from source
WORKDIR /tmp
RUN git clone --depth 1 https://github.com/libsdl-org/SDL.git SDL3 \
    && cd SDL3 \
    && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
    && make -j$(nproc) \
    && make install \
    && cd /tmp \
    && rm -rf SDL3

# Copy SDL3 libs to Emscripten's system directory for WASM compilation
RUN mkdir -p /emsdk/system/lib \
    && cp /usr/local/lib/libSDL3* /emsdk/system/lib/

# Optional: If you need SDL3_image, SDL3_mixer, SDL3_ttf, you'll need to compile these too
# Here's an example for SDL3_image:
# RUN git clone --depth 1 --branch SDL3 https://github.com/libsdl-org/SDL_image.git \
#     && cd SDL_image \
#     && mkdir build && cd build \
#     && cmake .. -DCMAKE_BUILD_TYPE=Release \
#     && make -j$(nproc) \
#     && make install \
#     && cd /tmp \
#     && rm -rf SDL_image

# Set working directory for your project
WORKDIR /app