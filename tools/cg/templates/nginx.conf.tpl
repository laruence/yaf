# nginx equivalent of public/.htaccess for {&$APP_NAME&}.
# Set root to the public/ directory so that only public assets are
# reachable, and forward every request to index.php.
server {
  listen 80;
  server_name domain.com;
  root   /path/to/{&$APP_NAME&}/public;
  index  index.php index.html index.htm;

  location / {
    try_files $uri $uri/ /index.php?$args;
  }

  location ~ \.php$ {
    include fastcgi_params;
    fastcgi_pass 127.0.0.1:9000;
    fastcgi_index index.php;
    fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
  }
}
