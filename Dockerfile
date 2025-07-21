# Dockerfile (VERSÃO CORRIGIDA E MAIS ROBUSTA)

# Imagem base: Ubuntu 24.04 LTS (Noble Numbat)
FROM ubuntu:24.04

# Evita que os instaladores peçam inputs durante o build
ENV DEBIAN_FRONTEND=noninteractive

# Argumentos para criar um usuário com o mesmo UID/GID do seu usuário host
ARG USER_UID=1000
ARG USER_GID=1000
ARG USERNAME=devuser

# Instalação de dependências
RUN apt-get update && apt-get install -y --no-install-recommends \
    # Utilitários básicos e de compilação
    sudo git wget curl nano bash-completion build-essential cmake pkg-config gdb \
    cppcheck valgrind \
    \
    # PACOTES PARA O SERVIDOR DE SINALIZAÇÃO PYTHON
    python3 python3-pip \
    \
    # PACOTES Essenciais para rede segura (HTTPS/WSS para cpp-httplib)
    ca-certificates \
    libssl-dev \
    \
    # GStreamer Core e Plugins Essenciais
    gstreamer1.0-tools libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-nice \
    \
    # Dependências para Gráficos e Aceleração de Hardware
    gstreamer1.0-gl \
    gstreamer1.0-vaapi mesa-va-drivers \
    libgtk-3-dev libx11-dev libwayland-dev x11-apps \
    \
    # Dependências para Áudio com PipeWire
    pipewire libspa-0.2-modules gstreamer1.0-pipewire \
    \
    # --- Dependências Específicas e Verificadas para WebRTC ---
    libgstreamer-plugins-bad1.0-dev \
    libnice-dev \
    libsrtp2-dev \
    libglib2.0-dev \
    \
    # Bibliotecas do python para sinalização
    python3-websockets \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Script inteligente para criar ou modificar o usuário e grupo
RUN set -x && \
    if getent passwd ${USER_UID} >/dev/null 2>&1; then deluser --remove-home $(getent passwd ${USER_UID} | cut -d: -f1); fi && \
    if getent group ${USER_GID} >/dev/null 2>&1; then delgroup $(getent group ${USER_GID} | cut -d: -f1); fi && \
    groupadd --gid ${USER_GID} ${USERNAME} && \
    useradd --uid ${USER_UID} --gid ${USER_GID} -m -s /bin/bash ${USERNAME} && \
    usermod -aG sudo ${USERNAME} && \
    echo "${USERNAME} ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/${USERNAME}

RUN groupadd --gid 985 dri && groupadd --gid 989 render && \
    usermod -aG dri,render,video ${USERNAME}
# Define o usuário padrão do contêiner
USER ${USERNAME}

# Define o diretório de trabalho
WORKDIR /workspaces/learn_gstreamer

# Comando padrão para manter o contêiner rodando
CMD ["/bin/bash"]