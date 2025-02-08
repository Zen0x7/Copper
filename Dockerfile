FROM iantorres/boosted:1.87.0-debug

ARG CODECOV_TOKEN=NONE
ARG CODECOV_REPO=NONE
ARG COMMIT_SHA=NONE

COPY . .

RUN curl -Os https://cli.codecov.io/latest/linux/codecov  \
    && chmod +x codecov \
    && apt update \
    && apt install -y lcov

RUN mkdir build \
    && cd build \
    && cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug \
    && make \
    && cd ../bin \
    && ./tests \
    && cd .. \
    && lcov --capture --directory . --output-file coverage.info \
    && lcov --list coverage.info \
    && ./codecov --verbose upload-process --fail-on-error -t $CODECOV_TOKEN -f coverage.info --slug=$CODECOV_REPO --sha $COMMIT_SHA || echo "Codecov did not collect coverage reports"