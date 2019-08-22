#include <cstdio>
#include <fstream>
#include <string>

#include "googletest/include/gtest/gtest.h"
#include "iris_advanced_toolkit/ply_reader.h"
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
  ASSERT_EQ(1u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);

  FreePlyData(ply_data);
}

TEST(PlyReaderTests, OneQuad) {
  TempFile file("OneQuad.ply");
  file.Append("element vertex 4");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("element face 1");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0");
  file.Append("0 1 0");
  file.Append("1 1 0");
  file.Append("1 0 0");
  file.Append("4 0 1 2 3");

  PPLY_DATA ply_data;
  ISTATUS status = ReadFromPlyFile(file.FileName(), &ply_data);
  ASSERT_EQ(ISTATUS_SUCCESS, status);
  ASSERT_EQ(4u, ply_data->num_vertices);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
            ply_data->vertices[0]);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
            ply_data->vertices[1]);
  EXPECT_EQ(PointCreate((float_t)1.0, (float_t)1.0, (float_t)0.0),
            ply_data->vertices[2]);
  EXPECT_EQ(PointCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
            ply_data->vertices[3]);
  EXPECT_EQ(NULL, ply_data->normals);
  EXPECT_EQ(NULL, ply_data->uvs);
  ASSERT_EQ(2u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);
  EXPECT_EQ(0u, ply_data->faces[3]);
  EXPECT_EQ(2u, ply_data->faces[4]);
  EXPECT_EQ(3u, ply_data->faces[5]);

  FreePlyData(ply_data);
}

TEST(PlyReaderTests, TwoTriangles) {
  TempFile file("TwoTriangles.ply");
  file.Append("element vertex 4");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("element face 2");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0");
  file.Append("0 1 0");
  file.Append("1 1 0");
  file.Append("1 0 0");
  file.Append("3 0 1 2");
  file.Append("3 0 2 3");

  PPLY_DATA ply_data;
  ISTATUS status = ReadFromPlyFile(file.FileName(), &ply_data);
  ASSERT_EQ(ISTATUS_SUCCESS, status);
  ASSERT_EQ(4u, ply_data->num_vertices);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
            ply_data->vertices[0]);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
            ply_data->vertices[1]);
  EXPECT_EQ(PointCreate((float_t)1.0, (float_t)1.0, (float_t)0.0),
            ply_data->vertices[2]);
  EXPECT_EQ(PointCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
            ply_data->vertices[3]);
  EXPECT_EQ(NULL, ply_data->normals);
  EXPECT_EQ(NULL, ply_data->uvs);
  ASSERT_EQ(2u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);
  EXPECT_EQ(0u, ply_data->faces[3]);
  EXPECT_EQ(2u, ply_data->faces[4]);
  EXPECT_EQ(3u, ply_data->faces[5]);

  FreePlyData(ply_data);
}

TEST(PlyReaderTests, TwoQuads) {
  TempFile file("TwoTriangles.ply");
  file.Append("element vertex 6");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("element face 2");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0");
  file.Append("0 1 0");
  file.Append("1 1 0");
  file.Append("1 0 0");
  file.Append("1 -1 0");
  file.Append("0 -1 0");
  file.Append("4 0 1 2 3");
  file.Append("4 0 3 4 5");

  PPLY_DATA ply_data;
  ISTATUS status = ReadFromPlyFile(file.FileName(), &ply_data);
  ASSERT_EQ(ISTATUS_SUCCESS, status);
  ASSERT_EQ(6u, ply_data->num_vertices);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
            ply_data->vertices[0]);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
            ply_data->vertices[1]);
  EXPECT_EQ(PointCreate((float_t)1.0, (float_t)1.0, (float_t)0.0),
            ply_data->vertices[2]);
  EXPECT_EQ(PointCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
            ply_data->vertices[3]);
  EXPECT_EQ(PointCreate((float_t)1.0, (float_t)-1.0, (float_t)0.0),
            ply_data->vertices[4]);
  EXPECT_EQ(PointCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
            ply_data->vertices[5]);
  EXPECT_EQ(NULL, ply_data->normals);
  EXPECT_EQ(NULL, ply_data->uvs);
  ASSERT_EQ(4u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);
  EXPECT_EQ(0u, ply_data->faces[3]);
  EXPECT_EQ(2u, ply_data->faces[4]);
  EXPECT_EQ(3u, ply_data->faces[5]);
  EXPECT_EQ(0u, ply_data->faces[6]);
  EXPECT_EQ(3u, ply_data->faces[7]);
  EXPECT_EQ(4u, ply_data->faces[8]);
  EXPECT_EQ(0u, ply_data->faces[9]);
  EXPECT_EQ(4u, ply_data->faces[10]);
  EXPECT_EQ(5u, ply_data->faces[11]);

  FreePlyData(ply_data);
}

TEST(PlyReaderTests, OneTriangleWithNormals) {
  TempFile file("OneTriangleWithNormals.ply");
  file.Append("element vertex 3");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("property float32 nx");
  file.Append("property float32 ny");
  file.Append("property float32 nz");
  file.Append("element face 1");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0 1 0 0");
  file.Append("0 1 0 0 1 0");
  file.Append("1 0 0 0 0 1");
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
  EXPECT_EQ(VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
            ply_data->normals[0]);
  EXPECT_EQ(VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
            ply_data->normals[1]);
  EXPECT_EQ(VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
            ply_data->normals[2]);
  EXPECT_EQ(NULL, ply_data->uvs);
  ASSERT_EQ(1u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);

  FreePlyData(ply_data);
}

TEST(PlyReaderTests, OneTriangleWithUvs) {
  TempFile file("OneTriangleWithUvs.ply");
  file.Append("element vertex 3");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("property float32 u");
  file.Append("property float32 v");
  file.Append("element face 1");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0 0 0");
  file.Append("0 1 0 1 0");
  file.Append("1 0 0 0 1");
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
  EXPECT_EQ((float_t)0.0, ply_data->uvs[0]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[1]);
  EXPECT_EQ((float_t)1.0, ply_data->uvs[2]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[3]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[4]);
  EXPECT_EQ((float_t)1.0, ply_data->uvs[5]);
  ASSERT_EQ(1u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);

  FreePlyData(ply_data);
}

TEST(PlyReaderTests, OneTriangleWithSts) {
  TempFile file("OneTriangleWithSts.ply");
  file.Append("element vertex 3");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("property float32 s");
  file.Append("property float32 t");
  file.Append("element face 1");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0 0 0");
  file.Append("0 1 0 1 0");
  file.Append("1 0 0 0 1");
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
  EXPECT_EQ((float_t)0.0, ply_data->uvs[0]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[1]);
  EXPECT_EQ((float_t)1.0, ply_data->uvs[2]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[3]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[4]);
  EXPECT_EQ((float_t)1.0, ply_data->uvs[5]);
  ASSERT_EQ(1u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);

  FreePlyData(ply_data);
}

TEST(PlyReaderTests, OneTriangleWithTextureUvs) {
  TempFile file("OneTriangleWithTextureUvs.ply");
  file.Append("element vertex 3");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("property float32 texture_u");
  file.Append("property float32 texture_v");
  file.Append("element face 1");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0 0 0");
  file.Append("0 1 0 1 0");
  file.Append("1 0 0 0 1");
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
  EXPECT_EQ((float_t)0.0, ply_data->uvs[0]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[1]);
  EXPECT_EQ((float_t)1.0, ply_data->uvs[2]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[3]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[4]);
  EXPECT_EQ((float_t)1.0, ply_data->uvs[5]);
  ASSERT_EQ(1u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);

  FreePlyData(ply_data);
}

TEST(PlyReaderTests, OneTriangleWithTextureSts) {
  TempFile file("OneTriangleWithTextureSts.ply");
  file.Append("element vertex 3");
  file.Append("property float32 x");
  file.Append("property float32 y");
  file.Append("property float32 z");
  file.Append("property float32 texture_s");
  file.Append("property float32 texture_t");
  file.Append("element face 1");
  file.Append("property list uint8 int32 vertex_indices");
  file.Append("end_header");
  file.Append("0 0 0 0 0");
  file.Append("0 1 0 1 0");
  file.Append("1 0 0 0 1");
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
  EXPECT_EQ((float_t)0.0, ply_data->uvs[0]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[1]);
  EXPECT_EQ((float_t)1.0, ply_data->uvs[2]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[3]);
  EXPECT_EQ((float_t)0.0, ply_data->uvs[4]);
  EXPECT_EQ((float_t)1.0, ply_data->uvs[5]);
  ASSERT_EQ(1u, ply_data->num_faces);
  EXPECT_EQ(0u, ply_data->faces[0]);
  EXPECT_EQ(1u, ply_data->faces[1]);
  EXPECT_EQ(2u, ply_data->faces[2]);

  FreePlyData(ply_data);
}