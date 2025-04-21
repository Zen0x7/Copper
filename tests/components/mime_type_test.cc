// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <gtest/gtest.h>
#include <copper/components/mime_type.hpp>

using namespace copper::components;

class MimeTypeTestFixture : public ::testing::Test {
 protected:
  void expect_mime(const std::string& filename, const std::string& expected) {
    ASSERT_EQ(mime_type(filename), expected);
  }
};

TEST_F(MimeTypeTestFixture, KnownExtensions) {
  expect_mime("index.html", "text/html");
  expect_mime("style.css", "text/css");
  expect_mime("script.js", "application/javascript");
  expect_mime("file.txt", "text/plain");
  expect_mime("file.php", "text/html");
  expect_mime("file.htm", "text/html");
  expect_mime("file.json", "application/json");
  expect_mime("file.jpe", "image/jpeg");
  expect_mime("file.gif", "image/gif");
  expect_mime("file.bmp", "image/bmp");
  expect_mime("file.ico", "image/vnd.microsoft.icon");
  expect_mime("file.tiff", "image/tiff");
  expect_mime("file.tif", "image/tiff");
  expect_mime("file.svgz", "image/svg+xml");
  expect_mime("file.swf", "application/x-shockwave-flash");
  expect_mime("image.png", "image/png");
  expect_mime("photo.jpeg", "image/jpeg");
  expect_mime("photo.JPG", "image/jpeg");
  expect_mime("vector.svg", "image/svg+xml");
  expect_mime("video.flv", "video/x-flv");
}

TEST_F(MimeTypeTestFixture, UnknownOrMissingExtension) {
  expect_mime("data.bin", "application/text");
  expect_mime("README", "application/text");
  expect_mime("hidden.", "application/text");
}

TEST_F(MimeTypeTestFixture, CaseInsensitiveExtensions) {
  expect_mime("index.HTML", "text/html");
  expect_mime("document.Xml", "application/xml");
  expect_mime("style.CSS", "text/css");
}

TEST_F(MimeTypeTestFixture, NotListedCommonExtensions) {
  expect_mime("document.pdf", "application/text");
  expect_mime("archive.zip", "application/text");
  expect_mime("video.mp4", "application/text");
  expect_mime("data.csv", "application/text");
}