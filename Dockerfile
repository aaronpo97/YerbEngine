# Stage 1 — build
FROM emscripten/emsdk:latest AS builder
WORKDIR /app
COPY . .

RUN mkdir -p embuild && cd embuild && \
    emcmake cmake -DBUILD_SHARED_LIBS=OFF .. && \
    cmake --build .

# Stage 2 — hosting
FROM node:alpine
WORKDIR /app
COPY --from=builder /app/embuild/bin ./embuild/bin

RUN npm install -g serve

EXPOSE 3333
CMD ["serve", "-s", "embuild/bin", "-p", "3333"]
