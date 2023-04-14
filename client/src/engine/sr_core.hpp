#pragma once
#include <bits/stdc++.h>
#include <strstream>
#include <SDL.h>

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
		SDL_Texture* texture = NULL;
		color color = {255, 255, 255};

		bool LoadFile(std::string sFilename, bool bHasTexture = false);
	};

//-----------------------------------------------------------------------------
// [SECTION] Utils
//-----------------------------------------------------------------------------

	Uint8 LZ(int x);
	float Distance(vec3 vec1, vec3 vec2);
	float degToRad(float ang);
	float radToDeg(float rad);
	float FixAng(float ang);
	float FixY(float ang);

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
	int Triangle_ClipAgainstPlane(vec3 plane_p, vec3 plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2);
	vec3 Interpolate_Normal_i(ivec2 p[3], vec3 n[3], ivec2 fp);
	vec3 Interpolate_Normal(vec3 p[3], vec3 n[3], vec3 fp);
	ivec2 Interpolate_TextureCoords(ivec2 p[3], ivec2 t[3], ivec2 fp);
	color Interpolate_Color(ivec2 p[3], color c[3], ivec2 fp);
	void bresanham(int x1, int y1, int x2, int y2, std::function<void(int, int)>);
	void Raster(sr::ivec2 p[3], std::function<void(sr::ivec2)> pixel, int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void Raster_NonInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, color color);
	void Raster_ColorInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, color colors[3]);
	void Raster_NormalInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, color color, vec3 normals[3], vec3 light);
	void Raster_ColorAndNormalInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, color colors[3], vec3 normals[3], vec3 light);
	void Raster_TextureInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, color color, ivec2 TextureCoordonates);
	void Raster_FullyInterpolated(std::function<void(sr::ivec2, sr::color)> setpixel, ivec2 p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, color color[3], vec3 normals[3], ivec2 TextureCoordonates[3], vec3 light, SDL_Texture *texture);

//-----------------------------------------------------------------------------
// [SECTION] Matrix
//-----------------------------------------------------------------------------

	mat4x4 Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2);
	mat4x4 Matrix_MakeIdentity();
	mat4x4 Matrix_Projection(int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float Zfar, float Znear);
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

	void Render(std::function<void(sr::ivec2, sr::color)> setpixel, std::vector<mesh> mesh_collection, mat4x4 Projection_Matrix, player_t Camera, float *pDepthBuffer, vec3 light, int SCREEN_WIDTH, int SCREEN_HEIGHT, bool phong);
};
