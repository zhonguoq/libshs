class Shs < Formula
  desc "A C library for packaging directories into a single file"
  homepage "https://github.com/zhonguoq/libshs"
  url "https://github.com/zhonguoq/libshs/archive/refs/tags/v1.0.1.tar.gz"
  sha256 "e9628e15a620f39d06732b392188a57fb47af986cfa47feebe313fdc96d4a469"
  license "MIT"

  depends_on "cmake" => :build  # 如果使用 CMake 构建
  # depends_on "make" => :build  # 如果使用 Makefile 构建

  def install
    system "cmake", ".", *std_cmake_args
    system "make"
    system "make", "install"
  end
end

