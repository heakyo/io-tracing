FROM python:3-slim

WORKDIR /app
COPY server.py .

ENV BUILDS_DIR=/var/jenkins_home/buildweb_data
ENV BUILDWEB_PORT=9090

EXPOSE 9090

CMD ["python3", "server.py"]
