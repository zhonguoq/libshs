class Shs < Formula
  desc "A C library for packaging directories into a single file"
  homepage "https://github.com/zhonguoq/libshs"
  url "https://github.com/zhonguoq/libshs/archive/refs/tags/v1.0.0.tar.gz"
  sha256 "e38611f788ba28e882bda788e9b2785401830891a1eb267147db7402b2964d12"
  license "MIT"

  depends_on "cmake" => :build  # 如果使用 CMake 构建
  # depends_on "make" => :build  # 如果使用 Makefile 构建

  def install
    system "cmake", ".", *std_cmake_args
    system "make"
    system "make", "install"
  end
end

