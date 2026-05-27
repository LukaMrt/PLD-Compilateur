# Image de dev pour ifcc — reproduit l'environnement Linux/WSL attendu par le
# Makefile existant (config/config-wsl-2025.mk) sans rien y modifier.
#
# Build :  docker build -t ifcc-dev .
# Usage :  docker run --rm -v "$PWD":/work -w /work ifcc-dev make test
FROM --platform=linux/amd64 debian:stable-slim

# ANTLR reste en 4.13.2 : c'est la version figée dans config/config-wsl-2025.mk
# (nom du jar) que le Makefile réutilise tel quel dans le conteneur.
ARG ANTLR_VERSION=4.13.2

# Le Makefile choisit config-wsl-2025.mk dès qu'on est sous Linux, et ce fichier
# lit le jar dans /home/$(USER)/antlr4-install. On fige USER=root → /home/root.
ENV USER=root \
    DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        g++ make cmake pkg-config \
        default-jre-headless \
        python3 \
        curl ca-certificates \
        uuid-dev \
    && rm -rf /var/lib/apt/lists/*

# 1) Le jar ANTLR, au chemin exact attendu par config-wsl-2025.mk
# Le Makefile compose littéralement /home/$(USER)/... → /home/root/... ici.
RUN mkdir -p /home/root/antlr4-install && \
    curl -fSL "https://www.antlr.org/download/antlr-${ANTLR_VERSION}-complete.jar" \
        -o "/home/root/antlr4-install/antlr-${ANTLR_VERSION}-complete.jar"

# 2) Le runtime C++ ANTLR, compilé depuis les sources et installé dans /usr/local
#    (→ /usr/local/include/antlr4-runtime + /usr/local/lib/libantlr4-runtime.a)
RUN curl -fSL "https://github.com/antlr/antlr4/archive/refs/tags/${ANTLR_VERSION}.tar.gz" \
        -o /tmp/antlr4.tar.gz && \
    tar -xzf /tmp/antlr4.tar.gz -C /tmp && \
    cmake -S "/tmp/antlr4-${ANTLR_VERSION}/runtime/Cpp" -B /tmp/antlr4-build \
        -DCMAKE_BUILD_TYPE=Release \
        -DANTLR_BUILD_SHARED=OFF \
        -DANTLR_BUILD_CPP_TESTS=OFF \
        -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build /tmp/antlr4-build -j"$(nproc)" && \
    cmake --install /tmp/antlr4-build && \
    rm -rf /tmp/antlr4.tar.gz /tmp/antlr4-${ANTLR_VERSION} /tmp/antlr4-build

WORKDIR /work
CMD ["make", "test"]
