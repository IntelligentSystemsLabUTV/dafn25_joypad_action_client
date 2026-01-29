# !/bin/sh
# docker compose -f 'docker/container-x86-dev/.devcontainer/docker-compose.yaml' stop 
docker compose -f 'docker/container-x86-dev/.devcontainer/docker-compose.yaml' up -d
docker compose -f 'docker/container-x86-dev/.devcontainer/docker-compose.yaml' exec -it dafn25_action_client-x86-dev zsh 

