FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential cmake git curl \
    libssl-dev libpq-dev libpqxx-dev \
    libboost-system-dev libboost-date-time-dev \
    nlohmann-json3-dev && \
    rm -rf /var/lib/apt/lists/*

# Crow — header only v1.0+5
RUN curl -L https://github.com/CrowCpp/Crow/releases/download/v1.0%2B5/crow_all.h \
    -o /usr/local/include/crow_all.h

# jwt-cpp — header only
RUN git clone --depth=1 https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp && \
    cp -r /tmp/jwt-cpp/include/jwt-cpp /usr/local/include/jwt-cpp && \
    rm -rf /tmp/jwt-cpp

WORKDIR /app
COPY CMakeLists.txt .
COPY src/ src/

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j$(nproc)

# ── Runtime stage ─────────────────────────────────────────────
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    libpqxx-6.4 libssl3 libboost-system1.74.0 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/build/smartcampus /app/smartcampus
EXPOSE 8080
CMD ["/app/smartcampus"]
