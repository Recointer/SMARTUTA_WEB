FROM nginx:alpine
# Install openssl to generate self-signed cert at build time
RUN apk add --no-cache openssl && \
    mkdir -p /etc/nginx/ssl && \
    openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
      -keyout /etc/nginx/ssl/key.pem \
      -out    /etc/nginx/ssl/cert.pem \
      -subj "/C=EC/ST=Tungurahua/L=Ambato/O=UTA/CN=smartcampus.uta.edu.ec"
COPY nginx.conf /etc/nginx/conf.d/default.conf
COPY public/ /usr/share/nginx/html/
EXPOSE 80 443
