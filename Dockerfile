# syntax=docker/dockerfile:1

FROM ubuntu:jammy as builder

RUN apt update && \
	apt install -y \
		clang \
		cmake \
		curl \
		default-jre-headless \
		git \
		ninja-build \
		pkg-config \
		zip && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*

COPY . /src

RUN cmake -S /src -B /build/Release -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_CXX_COMPILER=clang++ -G Ninja && \
	cmake --build /build/Release/ && \
	mkdir -p /app/bin && \
	cp /build/Release/out/bin/rinha-de-compiler /app/bin/


FROM ubuntu:jammy

COPY --from=builder /app /app

CMD [ "/app/bin/rinha-de-compiler", "/var/rinha/source.rinha"]
