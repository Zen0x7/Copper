FROM iantorres/boosted:1.87.0-debug

COPY . .

RUN mkdir build \
    && cd build \
    && cmake .. -DBUILD_TESTS=ON \
    && make \
    && cd ../bin \
    && ./tests