FROM gcc:13 AS build
WORKDIR /app
COPY . .
RUN g++ -std=c++17 -O2 -I include -pthread main.cpp DiagnosticEngine.cpp -o hantavirus_server

FROM debian:bookworm-slim
WORKDIR /app
COPY --from=build /app/hantavirus_server .
# Render sets $PORT at runtime; the app reads it directly (see main.cpp).
CMD ["./hantavirus_server"]
