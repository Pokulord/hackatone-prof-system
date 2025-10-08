# Stage 1: Builder
# This stage compiles the C++ application
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libpq-dev \
    libssl-dev \
    pkg-config

# Set the working directory
WORKDIR /app

# Copy the entire project
COPY . .

# Configure and build the application
RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build

# Stage 2: Final Image
# This stage creates the final, lightweight image
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libpq5 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the compiled executable from the builder stage
COPY --from=builder /app/build/AuthServer .

# Copy configuration and migration files
COPY .env .
COPY migrations ./migrations

# Expose the port the application runs on
EXPOSE 18080

# Command to run the application
CMD ["./AuthServer"]
