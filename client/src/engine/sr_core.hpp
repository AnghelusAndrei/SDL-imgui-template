#pragma once
#include <bits/stdc++.h>

namespace sr{

//-----------------------------------------------------------------------------
// [SECTION] Data types
//-----------------------------------------------------------------------------



	struct vec3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;

		vec3();
		vec3(float x_, float y_, float z_);
	};

	struct ivec2
	{
		int x = 0;
		int y = 0;

		ivec2();
		ivec2(int x_, int y_);
	};

	struct color
	{
		int r,g,b;

		color();
		color(int r_, int g_, int b_);
		color(float r_, float g_, float b_);
	};

	struct internal_buffer_object{
		float *depth_buffer;
		int *scanline_buffer;

		internal_buffer_object(sr::ivec2 framebuffer_size);
		~internal_buffer_object();
		void update_frame(sr::ivec2 framebuffer_size);
		void update_framebuffer_size(sr::ivec2 framebuffer_size);


	};

	struct player_t
	{
		float x, y, z, angx, angy;
	};

	struct triangle
	{
		vec3 p[3];
		vec3 t[3];
		color c;
		vec3 n[3];
		uint32_t vert_i[3];
	};

	struct mat4x4
	{
		float m[4][4] = { 0 };
	};

	struct mesh
	{
		std::vector<triangle> tris;

		vec3 position;
		vec3 size;
		vec3 rotation;
		color color = {255, 255, 255};

		char *texture;

		bool LoadFile(std::string sFilename);
	};

//-----------------------------------------------------------------------------
// [SECTION] Utils
//-----------------------------------------------------------------------------

	uint8_t LZ(int x);
	float degToRad(float ang);
	float radToDeg(float rad);

//-----------------------------------------------------------------------------
// [SECTION] Vector math
//-----------------------------------------------------------------------------

	vec3 Vector_Add(vec3 &v1, vec3 &v2);
	ivec2 Vector_Add_i(ivec2 &v1, ivec2 &v2);
	vec3 Vector_Sub(vec3 &v1, vec3 &v2);
	ivec2 Vector_Sub_i(ivec2 &v1, ivec2 &v2);
	vec3 Vector_Mul(vec3 &v1, float k);
	ivec2 Vector_Mul_i(ivec2 &v1, float k);
	vec3 Vector_Div(vec3 &v1, float k);
	float Vector_DotProduct(vec3 &v1, vec3 &v2);
	float Vector_DotProduct_i(ivec2 &v1, ivec2 &v2);
	float Vector_Length(vec3 &v);
	float Vector_Length_i(ivec2 &v);
	float Distance(vec3 vec1, vec3 vec2);
	vec3 Vector_Normalise(vec3 &v);
	float dist(vec3 &p, vec3 &plane_n, vec3 &plane_p);
	vec3 Vector_CrossProduct(vec3 a, vec3 b);
	vec3 Vector_IntersectPlane(vec3 &plane_p, vec3 &plane_n, vec3 &lineStart, vec3 &lineEnd, float &t);
	vec3 Vector_MultiplyMatrix(vec3 &i, mat4x4 &m);


//-----------------------------------------------------------------------------
// [SECTION] Triangle
//-----------------------------------------------------------------------------


	float Triangle_Area(vec3 p[3]);
	float Triangle_Area_i(ivec2 p[3]);
	void OrderVertexIndex(vec3 p[4], int v[4]); 
	int Triangle_ClipAgainstPlane(vec3 plane_p, vec3 plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2);
	vec3 Interpolate_Normal_i(ivec2 p[3], vec3 n[3], ivec2 fp);
	vec3 Interpolate_Normal(vec3 p[3], vec3 n[3], vec3 fp);
	float Interpolate_Depth(ivec2 p[3], float d[3], ivec2 fp);
	ivec2 Interpolate_TextureCoords(ivec2 p[3], ivec2 t[3], ivec2 fp);
	color Interpolate_Color(ivec2 p[3], color c[3], ivec2 fp);
	void bresanham(int x1, int y1, int x2, int y2, std::function<void(int, int)>);
	void Raster(sr::ivec2 p[3], float depth[3], std::function<void(sr::ivec2)> pixel, sr::internal_buffer_object *buffers, sr::ivec2 framebuffer_size);
	void Raster_NonInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], float depth[3], sr::internal_buffer_object *buffers, sr::ivec2 framebuffer_size, color color);
	void Raster_ColorInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], float depth[3], sr::internal_buffer_object *buffers, sr::ivec2 framebuffer_size, color colors[3]);
	void Raster_NormalInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], float depth[3], sr::internal_buffer_object *buffers, sr::ivec2 framebuffer_size, color color, vec3 normals[3], vec3 light);
	void Raster_ColorAndNormalInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], float depth[3], sr::internal_buffer_object *buffers, sr::ivec2 framebuffer_size, color colors[3], vec3 normals[3], vec3 light);

//-----------------------------------------------------------------------------
// [SECTION] Matrix
//-----------------------------------------------------------------------------

	mat4x4 Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2);
	mat4x4 Matrix_MakeIdentity();
	mat4x4 Matrix_Projection(sr::ivec2 framebuffer_size, float FOV, float Zfar, float Znear);
	mat4x4 Translate(vec3 v2);
	mat4x4 Scale(vec3 v2);
	mat4x4 Matrix_RotateX(float ang);
	mat4x4 Matrix_RotateY(float ang);
	mat4x4 Matrix_RotateZ(float ang);
	mat4x4 Matrix_CameraPos(player_t &pos, vec3 &target, vec3 &up);
	mat4x4 Matrix_QuickInverse(mat4x4 &m);

//-----------------------------------------------------------------------------
// [SECTION] Rendering
//-----------------------------------------------------------------------------

	void Render(std::function<void(sr::ivec2, sr::color)> setpixel, std::vector<mesh*> mesh_collection, mat4x4 Projection_Matrix, player_t Camera, sr::internal_buffer_object *buffers, vec3 light, sr::ivec2 framebuffer_size, bool phong);
};
