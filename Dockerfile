#
# Docker image for ImmortalThreads
# Use Ubuntu 20.04
#

#
# Stage: Install MSP430 gcc toolchain. See homepage https://www.ti.com/tool/MSP430-GCC-OPENSOURCE
#
FROM ubuntu:focal as msp430_toolchain_fetcher
# See https://github.com/moby/moby/issues/4032
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
	apt-get install -y \
	wget \
	p7zip-full

RUN wget https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-LlCjWuAbzH/9.3.1.2/msp430-gcc-full-linux-x64-installer-9.3.1.2.7z
# Verify checksum
RUN md5sum msp430-gcc-full-linux-x64-installer-9.3.1.2.7z | grep 2db2f99b4cd5c541ca0389ee20c67527
# Extract
RUN 7z e msp430-gcc-full-linux-x64-installer-9.3.1.2.7z
# Install
RUN ./msp430-gcc-full-linux-x64-installer-9.3.1.2.run --mode unattended --prefix /opt/msp430-gcc/

#
# Stage: Dev environment
#
FROM ubuntu:focal

ARG DEBIAN_FRONTEND=noninteractive

# Install dev depenencies
RUN apt-get update && \
	apt-get install -y \
	# Build toolchain
	cmake \
	clang-10 \
	gcc-9 \
	g++-9 \
	# Required libraries
	llvm-10-dev \
	libclang-10-dev \
	libfmt-dev \
	# Tooling
	python3 \
	python3-pip \
	clang-format-10 \
	git \
	lcov

# Copy the installed MSP430 toolchain of the previous stage.
COPY --from=msp430_toolchain_fetcher /opt/ /opt/

COPY . /immortalthreads/
WORKDIR /immortalthreads/
