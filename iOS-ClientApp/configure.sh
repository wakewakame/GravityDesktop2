#!/bin/bash

set -eu

if ! [ -x ./3rdparty/grpc-swift/protoc-gen-swift ]; then
	cd 3rdparty/grpc-swift
	make plugins
	cd ../../
fi

protoc \
	--plugin="$(pwd)/3rdparty/grpc-swift/protoc-gen-swift" \
	--plugin="$(pwd)/3rdparty/grpc-swift/protoc-gen-grpc-swift" \
	--swift_out=. \
	--grpc-swift_out=. \
	--grpc-swift_opt=Client=true,Server=false \
	./gRPC-Client/imu.proto

xcodegen
