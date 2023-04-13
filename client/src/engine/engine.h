#pragma once
#include <bits/stdc++.h>
#include <strstream>
#include <SDL.h>

namespace sr{
	struct vec3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;

		vec3(){
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}
		vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_){}
	};


	struct color
	{
		int r,g,b;
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

	Uint8 LZ(int x){
		int z = x;
		if(z < 0){
			z = 0;
		}
		return z;
	}


	float Distance(vec3 vec1, vec3 vec2){
		return sqrt((vec1.x-vec2.x) * (vec1.x-vec2.x) + (vec1.y-vec2.y)*(vec1.y-vec2.y));
	}


	float degToRad(float ang)
	{
		return ang * (M_PI/180);;
	}


	float radToDeg(float rad)
	{
		return rad * (180/M_PI);
	}


	float FixAng(float ang)
	{
		if (ang > 359)
		{
			float mult = floor(ang / 360);
			ang = ang - (360 * mult);
		}
		if (ang < 0)
		{
			float ap = fabs(ang);
			float mult = ceil(ap / 360);
			ang = (360 * mult) - ap;
		}
		return ang;
	}


	float FixY(float ang){
		if (ang > 89)
		{
			ang = 89;
		}
		if (ang < -89)
		{
			ang = -89;
		}
		return ang;
	}


	vec3 Vector_Add(vec3 &v1, vec3 &v2)
	{
		vec3 nv;
		nv.x = v1.x + v2.x;
		nv.y = v1.y + v2.y;
		nv.z = v1.z + v2.z;
		return nv;
	}


	vec3 Vector_Sub(vec3 &v1, vec3 &v2)
	{
		vec3 nv;
		nv.x = v1.x - v2.x;
		nv.y = v1.y - v2.y;
		nv.z = v1.z - v2.z;
		return nv;
	}


	vec3 Vector_Mul(vec3 &v1, float k)
	{
		vec3 nv;
		nv.x = v1.x * k;
		nv.y = v1.y * k;
		nv.z = v1.z * k;
		return nv;
	}


	vec3 Vector_Div(vec3 &v1, float k)
	{
		vec3 nv;
		nv.x = v1.x / k;
		nv.y = v1.y / k;
		nv.z = v1.z / k;
		return nv;
	}


	float Vector_DotProduct(vec3 &v1, vec3 &v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}


	float Vector_Length(vec3 &v)
	{
		return sqrtf(Vector_DotProduct(v, v));
	}


	vec3 Vector_Normalise(vec3 &v)
	{ 
		float l = Vector_Length(v);
		vec3 nv;
		nv.x = v.x / l;
		nv.y = v.y / l;
		nv.z = v.z / l;
		return nv;
	}


	float dist(vec3 &p, vec3 &plane_n, vec3 &plane_p){
		vec3 n = Vector_Normalise(p);
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
	}


	struct mesh
	{
		std::vector<triangle> tris;

		vec3 position;
		vec3 size;
		vec3 rotation;
		SDL_Texture* texture = NULL;
		color color = {255, 255, 255};

		bool LoadFile(std::string sFilename, bool bHasTexture = false)
		{
			std::ifstream f(sFilename);
			if (!f.is_open())
				return false;

			std::vector<vec3> verts;
			std::vector<vec3> texs;
			std::vector<vec3> normals;

			while (!f.eof())
			{
				char line[128];
				f.getline(line, 128);

				std::strstream s;
				s << line;

				char junk;

				if (line[0] == 'v')
				{
					if (line[1] == 't')
					{
						vec3 v;
						s >> junk >> junk >> v.x >> v.y;
						texs.push_back(v);
					}
					else
					{
						vec3 v;
						s >> junk >> v.x >> v.y >> v.z;
						verts.push_back(v);
						normals.push_back((vec3){0,0,0});
					}
				}

				if (line[0] == 'f')
				{
					triangle t;
					if (!bHasTexture)
					{
						int f[3];
						s >> junk >> f[0] >> f[1] >> f[2];
						t.p[0] = verts[f[0] - 1];
						t.p[1] = verts[f[1] - 1];
						t.p[2] = verts[f[2] - 1];

						t.vert_i[0] = f[0] - 1;
						t.vert_i[1] = f[1] - 1;
						t.vert_i[2] = f[2] - 1;
					}else{
						s >> junk;

						std::string tokens[6];
						int nTokenCount = -1;


						while (!s.eof())
						{
							char c = s.get();
							if (c == ' ' || c == '/')
								nTokenCount++;
							else
								tokens[nTokenCount].append(1, c);
						}

						tokens[nTokenCount].pop_back();

						t.p[0] = verts[stoi(tokens[0]) - 1];
						t.p[1] = verts[stoi(tokens[2]) - 1];
						t.p[2] = verts[stoi(tokens[4]) - 1];

						t.vert_i[0] = stoi(tokens[0]) - 1;
						t.vert_i[1] = stoi(tokens[2]) - 1;
						t.vert_i[2] = stoi(tokens[4]) - 1;

						t.t[0] = texs[stoi(tokens[1]) - 1];
						t.t[1] = texs[stoi(tokens[3]) - 1];
						t.t[2] = texs[stoi(tokens[5]) - 1];

					}

                    vec3 line1 = Vector_Sub(t.p[1], t.p[0]);
                    vec3 line2 = Vector_Sub(t.p[2], t.p[0]);

					vec3 normal;
					normal.x = line1.y * line2.z - line1.z * line2.y;
					normal.y = line1.z * line2.x - line1.x * line2.z;
					normal.z = line1.x * line2.y - line1.y * line2.x;

                    normal = Vector_Normalise(normal);

                    normals[t.vert_i[0]] = Vector_Add(normals[t.vert_i[0]],normal);
                    normals[t.vert_i[1]] = Vector_Add(normals[t.vert_i[1]],normal);
                    normals[t.vert_i[2]] = Vector_Add(normals[t.vert_i[2]],normal);

					tris.push_back(t);
				}
			}

			for(int i = 0; i < tris.size(); i++){
				for(int j = 0; j < 3; j++){
					if(Vector_Length(normals[tris[i].vert_i[j]]) != 0){
						tris[i].n[j] = Vector_Normalise(normals[tris[i].vert_i[j]]);
					}
				}
			}

			return true;
		}
	};

	vec3 Vector_CrossProduct(vec3 a, vec3 b){
		float ni=a.y*b.z-a.z*b.y;
		float nj=a.z*b.x-a.x*b.z;
		float nk=a.x*b.y-a.y*b.x;
		return vec3(ni,nj,nk);
	}


	float Triangle_Area(triangle t){
		vec3 AB = Vector_Sub(t.p[0], t.p[1]);
		vec3 AC = Vector_Sub(t.p[0], t.p[2]);
		vec3 n = Vector_CrossProduct(AB, AC);
		return Vector_Length(n)/2;
	}



	vec3 Vector_IntersectPlane(vec3 &plane_p, vec3 &plane_n, vec3 &lineStart, vec3 &lineEnd, float &t)
		{
			plane_n = Vector_Normalise(plane_n);
			float plane_d = -Vector_DotProduct(plane_n, plane_p);
			float ad = Vector_DotProduct(lineStart, plane_n);
			float bd = Vector_DotProduct(lineEnd, plane_n);
			t = (-plane_d - ad) / (bd - ad);
			vec3 lineStartToEnd = Vector_Sub(lineEnd, lineStart);
			vec3 lineToIntersect = Vector_Mul(lineStartToEnd, t);
			return Vector_Add(lineStart, lineToIntersect);
		}


	int Triangle_ClipAgainstPlane(vec3 plane_p, vec3 plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2)
		{
			plane_n = Vector_Normalise(plane_n);

			vec3* inside_points[3];  int nInsidePointCount = 0;
			vec3* outside_points[3]; int nOutsidePointCount = 0;
			vec3* inside_tex[3]; int nInsideTexCount = 0;
			vec3* outside_tex[3]; int nOutsideTexCount = 0;


			float d0 = dist(in_tri.p[0], plane_n, plane_p);
			float d1 = dist(in_tri.p[1], plane_n, plane_p);
			float d2 = dist(in_tri.p[2], plane_n, plane_p);

			if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0]; }
			else {
				outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0];
			}
			if (d1 >= 0) {
				inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1];
			}
			else {
				outside_points[nOutsidePointCount++] = &in_tri.p[1];  outside_tex[nOutsideTexCount++] = &in_tri.t[1];
			}
			if (d2 >= 0) {
				inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2];
			}
			else {
				outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2];
			}


			if (nInsidePointCount == 0)
			{

				return 0;
			}

			if (nInsidePointCount == 3)
			{
				out_tri1 = in_tri;

				return 1;
			}

			if (nInsidePointCount == 1 && nOutsidePointCount == 2)
			{
				out_tri1.c =  in_tri.c;

				out_tri1.p[0] = *inside_points[0];
				out_tri1.t[0] = *inside_tex[0];

				float t;
				out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
				out_tri1.t[1].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
				out_tri1.t[1].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
				out_tri1.t[1].z = t * (outside_tex[0]->z - inside_tex[0]->z) + inside_tex[0]->z;

				out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
				out_tri1.t[2].x = t * (outside_tex[1]->x - inside_tex[0]->x) + inside_tex[0]->x;
				out_tri1.t[2].y = t * (outside_tex[1]->y - inside_tex[0]->y) + inside_tex[0]->y;
				out_tri1.t[2].z = t * (outside_tex[1]->z - inside_tex[0]->z) + inside_tex[0]->z;

				return 1;
			}

			if (nInsidePointCount == 2 && nOutsidePointCount == 1)
			{
				out_tri1.c =  in_tri.c;

				out_tri2.c =  in_tri.c;

				out_tri1.p[0] = *inside_points[0];
				out_tri1.p[1] = *inside_points[1];
				out_tri1.t[0] = *inside_tex[0];
				out_tri1.t[1] = *inside_tex[1];

				float t;
				out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
				out_tri1.t[2].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
				out_tri1.t[2].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
				out_tri1.t[2].z = t * (outside_tex[0]->z - inside_tex[0]->z) + inside_tex[0]->z;

				out_tri2.p[0] = *inside_points[1];
				out_tri2.t[0] = *inside_tex[1];
				out_tri2.p[1] = out_tri1.p[2];
				out_tri2.t[1] = out_tri1.t[2];
				out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
				out_tri2.t[2].x = t * (outside_tex[0]->x - inside_tex[1]->x) + inside_tex[1]->x;
				out_tri2.t[2].y = t * (outside_tex[0]->y - inside_tex[1]->y) + inside_tex[1]->y;
				out_tri2.t[2].z = t * (outside_tex[0]->z - inside_tex[1]->z) + inside_tex[1]->z;
				return 2;
			}
			return 0;
		}


	vec3 Vector_MultiplyMatrix(vec3 &i, mat4x4 &m)
	{
		vec3 v;
		v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
		v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
		v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
		v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
		return v;
	}


	mat4x4 Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2)
	{
		mat4x4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
		return matrix;
	}


	mat4x4 Matrix_MakeIdentity()
	{
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}


	mat4x4 Matrix_Projection(int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float Zfar, float Znear)
	{
		mat4x4 matrix;
		matrix.m[0][0] = (SCREEN_HEIGHT/SCREEN_HEIGHT) * (1/tan(degToRad(FOV/2)));
		matrix.m[1][1] = (1/tan(degToRad(FOV/2)));
		matrix.m[2][2] = Zfar / (Zfar-Znear);
		matrix.m[3][2] = (-Zfar * Znear) / (Zfar - Znear);
		matrix.m[2][3] = 1;
		matrix.m[3][3] = 0;
		return matrix;
	}


	mat4x4 Translate(vec3 v2){
		mat4x4 matrix;

		matrix.m[0][0] = 1;
		matrix.m[1][1] = 1;
		matrix.m[2][2] = 1;
		matrix.m[3][3] = 1;
		matrix.m[3][0] = v2.x;
		matrix.m[3][1] = v2.y;
		matrix.m[3][2] = v2.z;

		return matrix;
	}


	mat4x4 Scale(vec3 v2){
		mat4x4 matrix;

		matrix.m[0][0] = v2.x;
		matrix.m[1][1] = v2.y;
		matrix.m[2][2] = v2.z;
		matrix.m[3][3] = 1;

		return matrix;
	}


	mat4x4 Matrix_RotateX(float ang)
	{
		float fAngleRad = degToRad(ang);
		mat4x4 matrix;
		matrix.m[0][0] = 1;
		matrix.m[1][1] = cos(fAngleRad);
		matrix.m[1][2] = sin(fAngleRad);
		matrix.m[2][1] = -sin(fAngleRad);
		matrix.m[2][2] = cos(fAngleRad);
		matrix.m[3][3] = 1;
		return matrix;
	}


	mat4x4 Matrix_RotateY(float ang)
	{
		mat4x4 matrix;
		matrix.m[0][0] = cos(degToRad(ang));
		matrix.m[1][1] = 1;
		matrix.m[0][2] = -sin(degToRad(ang));
		matrix.m[2][0] = sin(degToRad(ang));
		matrix.m[2][2] = cos(degToRad(ang));
		matrix.m[3][3] = 1;
		return matrix;
	}


	mat4x4 Matrix_RotateZ(float ang)
	{
		float fAngleRad = degToRad(ang);
		mat4x4 matrix;
		matrix.m[0][0] = cos(fAngleRad);
		matrix.m[0][1] = sin(fAngleRad);
		matrix.m[1][0] = -sin(fAngleRad);
		matrix.m[1][1] = cos(fAngleRad);
		matrix.m[2][2] = 1;
		matrix.m[3][3] = 1;
		return matrix;
	}


	mat4x4 Matrix_CameraPos(player_t &pos, vec3 &target, vec3 &up){

		vec3 newForward; 
		newForward.x = target.x-pos.x;
		newForward.y = target.y-pos.y;
		newForward.z = target.z-pos.z;
		float NFl = sqrt(newForward.x*newForward.x + newForward.y*newForward.y + newForward.z*newForward.z);
		newForward.x /= NFl; newForward.y /= NFl; newForward.z /= NFl;

		float NFUdp = up.x*newForward.x+up.y*newForward.y+up.z*newForward.z;
		vec3 a; 
		a.x = newForward.x*NFUdp;
		a.y = newForward.y*NFUdp;
		a.z = newForward.z*NFUdp;
		vec3 newUp; 
		newUp.x = up.x-a.x;
		newUp.y = up.y-a.y;
		newUp.z = up.z-a.z;
		float NUl = sqrt(newUp.x*newUp.x + newUp.y*newUp.y + newUp.z*newUp.z);
		newUp.x /= NUl; newUp.y /= NUl; newUp.z /= NUl;

		vec3 newRight; 
		newRight.x = newUp.y * newForward.z - newUp.z * newForward.y;
		newRight.y = newUp.z * newForward.x - newUp.x * newForward.z;
		newRight.z = newUp.x * newForward.y - newUp.y * newForward.x;

		mat4x4 matrix;
		matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0;
		matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0;
		matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0;
		matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1;
		return matrix;

	}


	mat4x4 Matrix_QuickInverse(mat4x4 &m)
	{
		mat4x4 matrix;
		matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0;
		matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0;
		matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0;
		matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1;
		return matrix;
	}


	void Frame(SDL_Renderer *renderer, std::vector<mesh> mesh_collection, mat4x4 Projection_Matrix, player_t Camera, float *pDepthBuffer, vec3 light, int SCREEN_WIDTH, int SCREEN_HEIGHT, bool phong){


		std::fill_n(pDepthBuffer, SCREEN_HEIGHT*SCREEN_WIDTH, 0.0f);

		float l_length = sqrt(light.x*light.x + light.y*light.y + light.z*light.z);
		light.x /= l_length; light.y /= l_length; light.z /= l_length;

		vec3 vLookDir;
		vLookDir.z = 1;
		mat4x4 LookRx, LookRy;

		LookRx = Matrix_RotateX(-Camera.angy);
		LookRy = Matrix_RotateY(Camera.angx);
		LookRy = Matrix_MultiplyMatrix(LookRx, LookRy);
		vLookDir = Vector_MultiplyMatrix(vLookDir, LookRy);

		vec3 pc1;
		vec3 pc2;
		vec3 pc3;
		vec3 pc4;
		vec3 pc5;
		vec3 pc6;
		vec3 pc7;
		pc2.y = 1;
		pc3.y = (float)SCREEN_HEIGHT - 1;
		pc4.y = -1;
		pc5.x = 1;
		pc6.x = (float)SCREEN_WIDTH - 1;
		pc7.x = -1;

		

		vec3 vUp;
		vUp.y = 1;
		vec3 vTarget; 
		vTarget.x = Camera.x + vLookDir.x;
		vTarget.y = Camera.y + vLookDir.y;
		vTarget.z = Camera.z + vLookDir.z;

		mat4x4 matCamera = Matrix_CameraPos(Camera, vTarget, vUp);
		mat4x4 matView = Matrix_QuickInverse(matCamera);

		for(int obj = 0; obj < mesh_collection.size(); obj++){
			mesh Mesh = mesh_collection[obj];

			mat4x4 RotX, RotY, RotZ, Size, Move, Mesh_Matrix;

			RotX = Matrix_RotateX(Mesh.rotation.x+180);
			RotY = Matrix_RotateY(Mesh.rotation.y);
			RotZ = Matrix_RotateZ(Mesh.rotation.z);
			Size = Scale(Mesh.size);
			Move = Translate(Mesh.position);

			Mesh_Matrix = Matrix_MultiplyMatrix(RotZ, RotY);
			Mesh_Matrix = Matrix_MultiplyMatrix(Mesh_Matrix, RotX);
			Mesh_Matrix = Matrix_MultiplyMatrix(Mesh_Matrix, Size);
			Mesh_Matrix = Matrix_MultiplyMatrix(Mesh_Matrix, Move);

			for(int i = 0; i < Mesh.tris.size(); i++){
				if(!phong){
					triangle transformation, projection, view;
					vec3 normal, line1, line2;

					transformation = Mesh.tris[i];

					for(int j = 0; j < 3; j++){
						transformation.p[j] = Vector_MultiplyMatrix(transformation.p[j], Mesh_Matrix);
					}

					line1 = Vector_Sub(transformation.p[1], transformation.p[0]);
					line2 = Vector_Sub(transformation.p[2], transformation.p[0]);

					normal.x = line1.y * line2.z - line1.z * line2.y;
					normal.y = line1.z * line2.x - line1.x * line2.z;
					normal.z = line1.x * line2.y - line1.y * line2.x;

					float l = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
					normal.x /= l; normal.y /= l; normal.z /= l;

					if(normal.x * (transformation.p[0].x - Camera.x) + normal.y * (transformation.p[0].y - Camera.y) + normal.z * (transformation.p[0].z - Camera.z) < 0)
					{

						float light_dp = normal.x*light.x+normal.y*light.y+normal.z*light.z;

						int shade = (int)(light_dp*250 + 5);
						if(shade < 5 || shade > 255){
							shade = 5;
						}

						for(int j = 0; j < 3; j++){
							view.p[j] = Vector_MultiplyMatrix(transformation.p[j], matView);
							view.t[j] = transformation.t[j];
						}


						int nClippedTriangles = 0;
						triangle clipped[2];
						vec3 p_1;
						vec3 p_2;
						p_2.z = 1;
						p_1.z = 0.1;
						
						nClippedTriangles = Triangle_ClipAgainstPlane(p_1, p_2, view, clipped[0], clipped[1]);

						for (int n = 0; n < nClippedTriangles; n++)
						{

							projection.p[0] = Vector_MultiplyMatrix(clipped[n].p[0], Projection_Matrix);
							projection.p[1] = Vector_MultiplyMatrix(clipped[n].p[1], Projection_Matrix);
							projection.p[2] = Vector_MultiplyMatrix(clipped[n].p[2], Projection_Matrix);

							projection.t[0] = clipped[n].t[0];
							projection.t[1] = clipped[n].t[1];
							projection.t[2] = clipped[n].t[2];

							projection.t[0].z = clipped[n].t[0].z;
							projection.t[1].z = clipped[n].t[1].z;
							projection.t[2].z = clipped[n].t[2].z;

							projection.t[0].x = projection.t[0].x / projection.p[0].w;
							projection.t[1].x = projection.t[1].x / projection.p[1].w;
							projection.t[2].x = projection.t[2].x / projection.p[2].w;

							projection.t[0].y = projection.t[0].y / projection.p[0].w;
							projection.t[1].y = projection.t[1].y / projection.p[1].w;
							projection.t[2].y = projection.t[2].y / projection.p[2].w;

							projection.t[0].z = 1/projection.p[0].w;
							projection.t[1].z = 1/projection.p[1].w;
							projection.t[2].z = 1/projection.p[2].w;

							projection.p[0] = Vector_Div(projection.p[0], projection.p[0].w);
							projection.p[1] = Vector_Div(projection.p[1], projection.p[1].w);
							projection.p[2] = Vector_Div(projection.p[2], projection.p[2].w);

							projection.c = clipped[n].c;

							projection.p[0].x *= 1;
							projection.p[1].x *= 1;
							projection.p[2].x *= 1;
							projection.p[0].y *= 1;
							projection.p[1].y *= 1;
							projection.p[2].y *= 1;

							projection.p[0].x += 1; projection.p[0].y += 1;
							projection.p[1].x += 1; projection.p[1].y += 1;
							projection.p[2].x += 1; projection.p[2].y += 1;

							projection.p[0].x *= SCREEN_HEIGHT*0.5; projection.p[0].y *= SCREEN_HEIGHT*0.5;
							projection.p[1].x *= SCREEN_HEIGHT*0.5; projection.p[1].y *= SCREEN_HEIGHT*0.5;
							projection.p[2].x *= SCREEN_HEIGHT*0.5; projection.p[2].y *= SCREEN_HEIGHT*0.5;

							projection.p[0].x += (SCREEN_WIDTH-SCREEN_HEIGHT)/2;
							projection.p[1].x += (SCREEN_WIDTH-SCREEN_HEIGHT)/2;
							projection.p[2].x += (SCREEN_WIDTH-SCREEN_HEIGHT)/2;

							projection.c = (color){shade, shade, shade};




							triangle clipped[2];
							std::list<triangle> listTriangles;

							listTriangles.push_back(projection);
							int nNewTriangles = 1;

							for (int p = 0; p < 4; p++)
							{
								int nTrisToAdd = 0;
								while (nNewTriangles > 0)
								{
									triangle test = listTriangles.front();
									listTriangles.pop_front();
									nNewTriangles--;

									switch (p)
									{
									case 0:	nTrisToAdd = Triangle_ClipAgainstPlane(pc1, pc2, test, clipped[0], clipped[1]); break;
									case 1:	nTrisToAdd = Triangle_ClipAgainstPlane(pc3, pc4, test, clipped[0], clipped[1]); break;
									case 2:	nTrisToAdd = Triangle_ClipAgainstPlane(pc1, pc5, test, clipped[0], clipped[1]); break;
									case 3:	nTrisToAdd = Triangle_ClipAgainstPlane(pc6, pc7, test, clipped[0], clipped[1]); break;
									}
									for (int w = 0; w < nTrisToAdd; w++)
										listTriangles.push_back(clipped[w]);
								}
								nNewTriangles = listTriangles.size();
							}


							for (std::list<triangle>::iterator t = listTriangles.begin(); t != listTriangles.end(); t++)
							{

								if(!Mesh.texture){
									const std::vector< SDL_Vertex > verts =
									{
										{ SDL_FPoint{ t->p[0].x, t->p[0].y }, SDL_Color{ LZ(t->c.r + Mesh.color.r - 255), LZ(t->c.g + Mesh.color.g - 255), LZ(t->c.b + Mesh.color.b - 255), 255 }, SDL_FPoint{ 0 }, },
										{ SDL_FPoint{ t->p[1].x, t->p[1].y }, SDL_Color{ LZ(t->c.r + Mesh.color.r - 255), LZ(t->c.g + Mesh.color.g - 255), LZ(t->c.b + Mesh.color.b - 255), 255 }, SDL_FPoint{ 0 }, },
										{ SDL_FPoint{ t->p[2].x, t->p[2].y }, SDL_Color{ LZ(t->c.r + Mesh.color.r - 255), LZ(t->c.g + Mesh.color.g - 255), LZ(t->c.b + Mesh.color.b - 255), 255 }, SDL_FPoint{ 0 }, },
									};
									SDL_RenderGeometry( renderer, nullptr, verts.data(), verts.size(), nullptr, 0 );
								}else{
									const std::vector< SDL_Vertex > verts =
									{
										{ SDL_FPoint{ t->p[0].x, t->p[0].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[0].x, t->t[0].y }, },
										{ SDL_FPoint{ t->p[1].x, t->p[1].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[1].x, t->t[1].y }, },
										{ SDL_FPoint{ t->p[2].x, t->p[2].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[2].x, t->t[2].y }, },
									};
									SDL_RenderGeometry( renderer, Mesh.texture, verts.data(), verts.size(), nullptr, 0 );
								}
							}
						}
					}
				}else {
					triangle transformation, projection, view;
					vec3 normal, line1, line2;

					transformation = Mesh.tris[i];

					for(int j = 0; j < 3; j++){
						transformation.p[j] = Vector_MultiplyMatrix(transformation.p[j], Mesh_Matrix);
					}

					line1 = Vector_Sub(transformation.p[1], transformation.p[0]);
					line2 = Vector_Sub(transformation.p[2], transformation.p[0]);

					normal.x = line1.y * line2.z - line1.z * line2.y;
					normal.y = line1.z * line2.x - line1.x * line2.z;
					normal.z = line1.x * line2.y - line1.y * line2.x;

					float l = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
					normal.x /= l; normal.y /= l; normal.z /= l;

					if(normal.x * (transformation.p[0].x - Camera.x) + normal.y * (transformation.p[0].y - Camera.y) + normal.z * (transformation.p[0].z - Camera.z) < 0)
					{

						float light_dp = normal.x*light.x+normal.y*light.y+normal.z*light.z;

						int shade = (int)(light_dp*250 + 5);
						if(shade < 5 || shade > 255){
							shade = 5;
						}

						for(int j = 0; j < 3; j++){
							view.p[j] = Vector_MultiplyMatrix(transformation.p[j], matView);
							view.t[j] = transformation.t[j];
						}


						int nClippedTriangles = 0;
						triangle clipped[2];
						vec3 p_1;
						vec3 p_2;
						p_2.z = 1;
						p_1.z = 0.1;
						
						nClippedTriangles = Triangle_ClipAgainstPlane(p_1, p_2, view, clipped[0], clipped[1]);

						for (int n = 0; n < nClippedTriangles; n++)
						{

							projection.p[0] = Vector_MultiplyMatrix(clipped[n].p[0], Projection_Matrix);
							projection.p[1] = Vector_MultiplyMatrix(clipped[n].p[1], Projection_Matrix);
							projection.p[2] = Vector_MultiplyMatrix(clipped[n].p[2], Projection_Matrix);

							projection.t[0] = clipped[n].t[0];
							projection.t[1] = clipped[n].t[1];
							projection.t[2] = clipped[n].t[2];

							projection.t[0].z = clipped[n].t[0].z;
							projection.t[1].z = clipped[n].t[1].z;
							projection.t[2].z = clipped[n].t[2].z;

							projection.t[0].x = projection.t[0].x / projection.p[0].w;
							projection.t[1].x = projection.t[1].x / projection.p[1].w;
							projection.t[2].x = projection.t[2].x / projection.p[2].w;

							projection.t[0].y = projection.t[0].y / projection.p[0].w;
							projection.t[1].y = projection.t[1].y / projection.p[1].w;
							projection.t[2].y = projection.t[2].y / projection.p[2].w;

							projection.t[0].z = 1/projection.p[0].w;
							projection.t[1].z = 1/projection.p[1].w;
							projection.t[2].z = 1/projection.p[2].w;

							projection.p[0] = Vector_Div(projection.p[0], projection.p[0].w);
							projection.p[1] = Vector_Div(projection.p[1], projection.p[1].w);
							projection.p[2] = Vector_Div(projection.p[2], projection.p[2].w);

							projection.c = clipped[n].c;

							projection.p[0].x *= 1;
							projection.p[1].x *= 1;
							projection.p[2].x *= 1;
							projection.p[0].y *= 1;
							projection.p[1].y *= 1;
							projection.p[2].y *= 1;

							projection.p[0].x += 1; projection.p[0].y += 1;
							projection.p[1].x += 1; projection.p[1].y += 1;
							projection.p[2].x += 1; projection.p[2].y += 1;

							projection.p[0].x *= SCREEN_HEIGHT*0.5; projection.p[0].y *= SCREEN_HEIGHT*0.5;
							projection.p[1].x *= SCREEN_HEIGHT*0.5; projection.p[1].y *= SCREEN_HEIGHT*0.5;
							projection.p[2].x *= SCREEN_HEIGHT*0.5; projection.p[2].y *= SCREEN_HEIGHT*0.5;

							projection.p[0].x += (SCREEN_WIDTH-SCREEN_HEIGHT)/2;
							projection.p[1].x += (SCREEN_WIDTH-SCREEN_HEIGHT)/2;
							projection.p[2].x += (SCREEN_WIDTH-SCREEN_HEIGHT)/2;

							projection.c = (color){shade, shade, shade};




							triangle clipped[2];
							std::list<triangle> listTriangles;

							listTriangles.push_back(projection);
							int nNewTriangles = 1;

							for (int p = 0; p < 4; p++)
							{
								int nTrisToAdd = 0;
								while (nNewTriangles > 0)
								{
									triangle test = listTriangles.front();
									listTriangles.pop_front();
									nNewTriangles--;

									switch (p)
									{
									case 0:	nTrisToAdd = Triangle_ClipAgainstPlane(pc1, pc2, test, clipped[0], clipped[1]); break;
									case 1:	nTrisToAdd = Triangle_ClipAgainstPlane(pc3, pc4, test, clipped[0], clipped[1]); break;
									case 2:	nTrisToAdd = Triangle_ClipAgainstPlane(pc1, pc5, test, clipped[0], clipped[1]); break;
									case 3:	nTrisToAdd = Triangle_ClipAgainstPlane(pc6, pc7, test, clipped[0], clipped[1]); break;
									}
									for (int w = 0; w < nTrisToAdd; w++)
										listTriangles.push_back(clipped[w]);
								}
								nNewTriangles = listTriangles.size();
							}


							for (std::list<triangle>::iterator t = listTriangles.begin(); t != listTriangles.end(); t++)
							{

								if(!Mesh.texture){
									const std::vector< SDL_Vertex > verts =
									{
										{ SDL_FPoint{ t->p[0].x, t->p[0].y }, SDL_Color{ LZ(t->c.r + Mesh.color.r - 255), LZ(t->c.g + Mesh.color.g - 255), LZ(t->c.b + Mesh.color.b - 255), 255 }, SDL_FPoint{ 0 }, },
										{ SDL_FPoint{ t->p[1].x, t->p[1].y }, SDL_Color{ LZ(t->c.r + Mesh.color.r - 255), LZ(t->c.g + Mesh.color.g - 255), LZ(t->c.b + Mesh.color.b - 255), 255 }, SDL_FPoint{ 0 }, },
										{ SDL_FPoint{ t->p[2].x, t->p[2].y }, SDL_Color{ LZ(t->c.r + Mesh.color.r - 255), LZ(t->c.g + Mesh.color.g - 255), LZ(t->c.b + Mesh.color.b - 255), 255 }, SDL_FPoint{ 0 }, },
									};
									SDL_RenderGeometry( renderer, nullptr, verts.data(), verts.size(), nullptr, 0 );
								}else{
									const std::vector< SDL_Vertex > verts =
									{
										{ SDL_FPoint{ t->p[0].x, t->p[0].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[0].x, t->t[0].y }, },
										{ SDL_FPoint{ t->p[1].x, t->p[1].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[1].x, t->t[1].y }, },
										{ SDL_FPoint{ t->p[2].x, t->p[2].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[2].x, t->t[2].y }, },
									};
									SDL_RenderGeometry( renderer, Mesh.texture, verts.data(), verts.size(), nullptr, 0 );
								}
							}
						}
					}
				}
			}
		}
	}
};
