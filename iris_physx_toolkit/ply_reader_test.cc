#include <cstdio>
#include <fstream>
#include <string>

#include "googletest/include/gtest/gtest.h"
#include "iris_physx_toolkit/ply_reader.h"
#include "test_util/equality.h"

namespace {

class TempFile {
 public:
  TempFile(const std::string& filename);
  ~TempFile();

  void Append(const std::string& line);
  const char* FileName() const;

 private:
  std::string m_filename;
};

TempFile::TempFile(const std::string& filename) : m_filename(filename) {
  std::ofstream outfile;
  outfile.open(m_filename, std::ios_base::trunc);
  outfile << "ply" << std::endl;
  outfile << "format ascii 1.0" << std::endl;
  outfile.close();
}

TempFile::~TempFile() { std::remove(m_filename.c_str()); }

void TempFile::Append(const std::string& line) {
  std::ofstream outfile;
  outfile.open(m_filename, std::ios_base::app);
  outfile << line << std::endl;
  outfile.close();
}

const char* TempFile::FileName() const { return m_filename.c_str(); }

}  // namespace

TEST(PlyReaderTests, OneTriangle) {
  TempFile file("OneTriangle.ply");
  file.Append("element vertex 3");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("element face 1");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0");
  file.Append("0 1 0");
  file.Append("1 0 0");
  file.Append("3 0 1 2");

  PPLY_DATA ply_data;
  ISTATUS status = ReadFromPlyFile(file.FileName(), &ply_data);
  ASSERT_EQ(ISTATUS_SUCCESS, status);
  ASSERT_EQ(3u, ply_data->num_vertices);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
            ply_data->vertices[0]);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
            ply_data->vertices[1]);
  EXPECT_EQ(PointCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
            ply_data->vertices[2]);
  EXPECT_EQ(NULL, ply_data->normals);
  EXPECT_EQ(NULL, ply_data->uvs);
  EXPECT_EQ(1u, ply_data->num_faces);

  FreePlyData(ply_data);
}