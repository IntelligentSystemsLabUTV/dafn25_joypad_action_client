# !/bin/sh
docker compose -f 'docker/container-x86-dev/.devcontainer/docker-compose.yaml' down 
docker compose -f 'docker/container-x86-dev/.devcontainer/docker-compose.yaml' up
