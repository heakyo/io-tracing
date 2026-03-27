FROM jenkins/jenkins:lts

USER root

# Copy gcc compiler and toolchain from the gcc image
COPY --from=gcc:latest /usr/local/bin/gcc /usr/local/bin/gcc
COPY --from=gcc:latest /usr/local/lib/ /usr/local/lib/
COPY --from=gcc:latest /usr/local/lib64/ /usr/local/lib64/
COPY --from=gcc:latest /usr/local/include/ /usr/local/include/
COPY --from=gcc:latest /usr/local/libexec/ /usr/local/libexec/

# Copy binutils (assembler, linker) and make
COPY --from=gcc:latest /usr/bin/as /usr/bin/as
COPY --from=gcc:latest /usr/bin/ld /usr/bin/ld
COPY --from=gcc:latest /usr/bin/make /usr/bin/make

# Copy C standard library headers and development libs
COPY --from=gcc:latest /usr/include/ /usr/include/
COPY --from=gcc:latest /usr/lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/
COPY --from=gcc:latest /usr/lib/gcc/ /usr/lib/gcc/

# Create cc symlink and update library cache
RUN ln -sf /usr/local/bin/gcc /usr/local/bin/cc && ldconfig

USER jenkins
