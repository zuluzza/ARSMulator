FROM ubuntu:20.04

RUN echo "--- Install dependencies ---"

RUN apt-get update && apt-get install ninja-build

RUN apt-get install -y cmake

RUN apt-get install -y build-essential

RUN apt-get install -y git

RUN git clone https://github.com/catchorg/Catch2.git 

RUN cd Catch2 && git checkout v3.1.0 && cmake -Bbuild -H. -DBUILD_TESTING=OFF && cmake --build build/ --target install

RUN echo "--- Copy files ---"

COPY . /tmp

RUN echo "--- Build ---"

RUN echo $(gcc --version)

RUN cd tmp && cmake -G "Ninja" CMakeLists.txt && cmake --build .

RUN echo "--- Start simulator ---"

RUN echo $(./tmp/src/build/cli_simulator -f /tmp/test/integration/source_code.s -c h,s,p,x2,p,r,p,m128,q > out.txt)

RUN echo "--- Check output ---"

RUN diff --strip-trailing-cr out.txt tmp/test/integration/expected_output.txt
