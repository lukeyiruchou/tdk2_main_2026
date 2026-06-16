FROM osrf/ros:humble-desktop

LABEL shell=bash

ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
# add user with default bash
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    && rm -rf /var/lib/apt/lists/*

# Install common tools
RUN apt-get update && apt-get install -y \
    curl \
    git \
    tree \
    net-tools \
    usbutils \
    tmux \
    vim \
    wget \
    python3-pip \
    ros-humble-rqt-tf-tree \
    bash-completion

RUN echo "source /opt/ros/humble/setup.bash" >> /etc/bash.bashrc \ 
    && echo "source /home/user/ros_tdk/ros2_ws/install/setup.bash" >> /etc/bash.bashrc

RUN apt-get update && apt-get dist-upgrade -y \
    && apt-get autoremove -y \
    && apt-get autoclean -y \
    && rm -rf /var/lib/apt/lists/*

ENV SHELL=/bin/bash
ENV TERM=xterm-256color

USER $USERNAME

CMD ["/bin/bash"]
