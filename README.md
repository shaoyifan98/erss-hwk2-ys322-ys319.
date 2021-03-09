# erss-hwk2-ys322-ys319

HTTP Caching Proxy
ys322 ys319

To run the proxy, go to docker-deploy directory, and run sudo docker-compose up.
The proxy.log file will be generated under docker-deploy/logs directory.
Our proxy supports GET, POST and CONNECT method.
We use a cache for GET responses according to validation and expiration rules.
