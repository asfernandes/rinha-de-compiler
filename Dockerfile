# syntax=docker/dockerfile:1

FROM ubuntu:jammy as builder

RUN apt update && \
	apt install -y \
		cmake \
		curl \
		default-jre-headless \
		g++ \
		git \
		ninja-build \
		pkg-config \
		zip && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*

COPY . /src

WORKDIR /src

RUN ([ -f ./vcpkg/bootstrap-vcpkg.sh ] || git submodule update --init) && \
	cmake -S . -B /build/Release -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON -G Ninja && \
	cmake --build /build/Release/ && \
	mkdir -p /app/bin && \
	cp /build/Release/out/bin/rinha-de-compiler /app/bin/


FROM ubuntu:jammy

COPY --from=builder /app /app

CMD [ "/app/bin/rinha-de-compiler", "/var/rinha/source.rinha"]
