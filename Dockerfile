FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

# Argumentos para passar o ID do usuário e do grupo do host para o build
ARG HOST_UID=1000
ARG HOST_GID=1000
ARG USERNAME=learn_gstreamer

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libssl-dev \
    gstreamer1.0-tools \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    libgstrtspserver-1.0-dev \
    neovim \
    sudo \
    libgtk-3-dev \
    libqt5x11extras5-dev \
    qtbase5-dev \
    pipewire \
    gstreamer1.0-pipewire \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -m 700 /tmp/runtime && chown root:root /tmp/runtime

# Etapa 3: Criar ou Modificar um Usuário para Corresponder ao UID/GID do Host
# Este script agora lida com conflitos de UID/GID, renomeando/modificando se necessário.
RUN export DEBIAN_FRONTEND=noninteractive \
    # Cria o grupo, se um com o GID não existir
    && if ! getent group $HOST_GID > /dev/null; then groupadd -g $HOST_GID learn_gstreamer; fi \
    # Garante que o usuário com o UID do host tenha o nome 'learn_gstreamer'
    && if getent passwd $HOST_UID > /dev/null; then \
        EXISTING_NAME=$(getent passwd $HOST_UID | cut -d: -f1); \
        if [ "$EXISTING_NAME" != "learn_gstreamer" ]; then \
            usermod -l learn_gstreamer -d /home/learn_gstreamer -m $EXISTING_NAME; \
        fi; \
    else \
        useradd -s /bin/bash -u $HOST_UID -g $HOST_GID -m learn_gstreamer; \
    fi \
    # Garante que o usuário tenha o GID correto e esteja no grupo sudo
    && usermod -g $HOST_GID learn_gstreamer \
    && usermod -aG sudo learn_gstreamer \
    && echo "learn_gstreamer ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/learn_gstreamer \
    && chmod 0440 /etc/sudoers.d/learn_gstreamer

# Etapa 4: Mudar para o Usuário e Configurar o Ambiente Python
# Agora que o usuário 'learn_gstreamer' GARANTIDAMENTE existe, podemos mudar para ele.
USER learn_gstreamer

WORKDIR /app

USER ${USERNAME}

CMD ["/bin/bash"]
