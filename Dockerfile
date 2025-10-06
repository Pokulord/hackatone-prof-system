# Stage 1: Build the application
FROM debian:bullseye as builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg && \
    /vcpkg/bootstrap-vcpkg.sh

# Install project dependencies
RUN /vcpkg/vcpkg install \
    openssl \
    bcrypt \
    crow \
    nlohmann-json \
    cpp-uuid

# Set up the build directory
WORKDIR /app

# Copy the source code
COPY . .

# Configure and build the project
RUN cmake -B build -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake .
RUN cmake --build build

# Stage 2: Create the final image
FROM debian:bullseye-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set up the application directory
WORKDIR /app

# Copy the compiled application from the builder stage
COPY --from=builder /app/build/MyApp .

# Expose the port the application runs on
EXPOSE 8080

# Run the application
CMD ["./MyApp"]
