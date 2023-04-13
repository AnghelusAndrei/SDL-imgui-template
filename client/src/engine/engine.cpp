#include "engine.hpp"

#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define x_off x - cx
#define y_off y - cy


//-----------------------------------------------------------------------------
// [SECTION] Utils
//-----------------------------------------------------------------------------

sr::vec3::vec3() : x(0.0f), y(.0f), z(0.0f), w(1.0f){}
sr::vec3::vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_){}
sr::vec2_i::vec2_i() : x(0), y(0){}
sr::vec2_i::vec2_i(int x_, int y_) : x(x_), y(y_){}
sr::color::color() : r(0), g(0), b(0){}
sr::color::color(int r_, int g_, int b_) : r(r_), g(g_), b(b_){}
sr::color::color(float r_, float g_, float b_) : r((int)r_), g((int)g_), b((int)b_){}

Uint8 sr::LZ(int x){
    int z = x;
    if(z < 0){
        z = 0;
    }
    return z;
}


float sr::Distance(sr::vec3 vec1, sr::vec3 vec2){
    return sqrt((vec1.x-vec2.x) * (vec1.x-vec2.x) + (vec1.y-vec2.y)*(vec1.y-vec2.y));
}


float sr::degToRad(float ang)
{
    return ang * (M_PI/180);;
}


float sr::radToDeg(float rad)
{
    return rad * (180/M_PI);
}


float sr::FixAng(float ang)
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


float sr::FixY(float ang){
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

//-----------------------------------------------------------------------------
// [SECTION] Vector math
//-----------------------------------------------------------------------------

sr::vec3 sr::Vector_Add(sr::vec3 &v1, sr::vec3 &v2)
{
    sr::vec3 nv;
    nv.x = v1.x + v2.x;
    nv.y = v1.y + v2.y;
    nv.z = v1.z + v2.z;
    return nv;
}

sr::vec2_i sr::Vector_Add_i(sr::vec2_i &v1, sr::vec2_i &v2)
{
    sr::vec2_i nv;
    nv.x = v1.x + v2.x;
    nv.y = v1.y + v2.y;
    return nv;
}



sr::vec3 sr::Vector_Sub(sr::vec3 &v1, sr::vec3 &v2)
{
    sr::vec3 nv;
    nv.x = v1.x - v2.x;
    nv.y = v1.y - v2.y;
    nv.z = v1.z - v2.z;
    return nv;
}

sr::vec2_i sr::Vector_Sub_i(sr::vec2_i &v1, sr::vec2_i &v2)
{
    sr::vec2_i nv;
    nv.x = v1.x - v2.x;
    nv.y = v1.y - v2.y;
    return nv;
}


sr::vec3 sr::Vector_Mul(sr::vec3 &v1, float k)
{
    sr::vec3 nv;
    nv.x = v1.x * k;
    nv.y = v1.y * k;
    nv.z = v1.z * k;
    return nv;
}

sr::vec2_i sr::Vector_Mul_i(sr::vec2_i &v1, float k)
{
    sr::vec2_i nv;
    nv.x = v1.x * k;
    nv.y = v1.y * k;
    return nv;
}

sr::vec3 sr::Vector_Div(sr::vec3 &v1, float k)
{
    sr::vec3 nv;
    nv.x = v1.x / k;
    nv.y = v1.y / k;
    nv.z = v1.z / k;
    return nv;
}


float sr::Vector_DotProduct(sr::vec3 &v1, sr::vec3 &v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float sr::Vector_DotProduct_i(sr::vec2_i &v1, sr::vec2_i &v2)
{
    return v1.x*v2.x + v1.y*v2.y;
}



float sr::Vector_Length(sr::vec3 &v)
{
    return sqrtf(Vector_DotProduct(v, v));
}

float sr::Vector_Length_i(sr::vec2_i &v)
{
    return sqrtf(Vector_DotProduct_i(v, v));
}

sr::vec3 sr::Vector_Normalise(sr::vec3 &v)
{ 
    float l = Vector_Length(v);
    if(!l)return v;
    sr::vec3 nv;
    nv.x = v.x / l;
    nv.y = v.y / l;
    nv.z = v.z / l;
    return nv;
}


float sr::dist(sr::vec3 &p, sr::vec3 &plane_n, sr::vec3 &plane_p){
    sr::vec3 n = Vector_Normalise(p);
    return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
}

//-----------------------------------------------------------------------------
// [SECTION] Mesh loading
//-----------------------------------------------------------------------------

bool sr::mesh::LoadFile(std::string sFilename, bool bHasTexture)
{
    std::ifstream f(sFilename);
    if (!f.is_open())
        return false;

    std::vector<sr::vec3> verts;
    std::vector<sr::vec3> texs;
    std::vector<sr::vec3> normals;

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
                sr::vec3 v;
                s >> junk >> junk >> v.x >> v.y;
                texs.push_back(v);
            }
            else
            {
                sr::vec3 v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
                normals.push_back((sr::vec3){0,0,0});
            }
        }

        if (line[0] == 'f')
        {
            sr::triangle t;
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

            sr::vec3 line1 = Vector_Sub(t.p[1], t.p[0]);
            sr::vec3 line2 = Vector_Sub(t.p[2], t.p[0]);

            sr::vec3 normal;
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

//-----------------------------------------------------------------------------
// [SECTION] Triangle utils
//-----------------------------------------------------------------------------

sr::vec3 sr::Vector_CrossProduct(sr::vec3 a, sr::vec3 b){
    float ni=a.y*b.z-a.z*b.y;
    float nj=a.z*b.x-a.x*b.z;
    float nk=a.x*b.y-a.y*b.x;
    return sr::vec3(ni,nj,nk);
}

float sr::Triangle_Area(sr::vec3 p[3]){
    sr::vec3 AB = Vector_Sub(p[0], p[1]);
    sr::vec3 AC = Vector_Sub(p[0], p[2]);
    sr::vec3 n = Vector_CrossProduct(AB, AC);
    return Vector_Length(n)/2;
}

float sr::Triangle_Area_i(sr::vec2_i p[3]){
    sr::vec2_i AB = Vector_Sub_i(p[0], p[1]);
    sr::vec2_i AC = Vector_Sub_i(p[0], p[2]);
    sr::vec3 n = Vector_CrossProduct(sr::vec3(AB.x, AB.y, 0), sr::vec3(AC.x, AC.y, 0));
    return Vector_Length(n)/2;
}



sr::vec3 sr::Vector_IntersectPlane(sr::vec3 &plane_p, sr::vec3 &plane_n, sr::vec3 &lineStart, sr::vec3 &lineEnd, float &t)
{
    plane_n = Vector_Normalise(plane_n);
    float plane_d = -Vector_DotProduct(plane_n, plane_p);
    float ad = Vector_DotProduct(lineStart, plane_n);
    float bd = Vector_DotProduct(lineEnd, plane_n);
    t = (-plane_d - ad) / (bd - ad);
    sr::vec3 lineStartToEnd = Vector_Sub(lineEnd, lineStart);
    sr::vec3 lineToIntersect = Vector_Mul(lineStartToEnd, t);
    return Vector_Add(lineStart, lineToIntersect);
}


sr::vec3 sr::Interpolate_Normal_i(sr::vec2_i p[3], sr::vec3 n[3], sr::vec2_i fp){
    sr::vec3 normal;

    float area = Triangle_Area_i(p);
    sr::vec2_i a1v[3] = (sr::vec2_i[3]){fp,p[1],p[2]};
    sr::vec2_i a2v[3] = (sr::vec2_i[3]){p[0],fp,p[2]};
    sr::vec2_i a3v[3] = (sr::vec2_i[3]){p[0],p[1],fp};
    float a1 = Triangle_Area_i(a1v)/area;
    float a2 = Triangle_Area_i(a2v)/area;
    float a3 = Triangle_Area_i(a3v)/area;

    sr::vec3 x = Vector_Mul(n[0],a1);
    sr::vec3 u = Vector_Mul(n[1],a2);
    sr::vec3 v = Vector_Mul(n[2],a3);
    sr::vec3 y = Vector_Add(u,v);

    normal = Vector_Add(x,y);

    return normal;
}

sr::vec3 sr::Interpolate_Normal(sr::vec3 p[3], sr::vec3 n[3], sr::vec3 fp){
    sr::vec3 normal;

    float area = Triangle_Area(p);
    if(!area)return n[0];
    sr::vec3 a1v[3] = (sr::vec3[3]){fp,p[1],p[2]};
    sr::vec3 a2v[3] = (sr::vec3[3]){p[0],fp,p[2]};
    sr::vec3 a3v[3] = (sr::vec3[3]){p[0],p[1],fp};
    float a1 = Triangle_Area(a1v)/area;
    float a2 = Triangle_Area(a2v)/area;
    float a3 = Triangle_Area(a3v)/area;

    sr::vec3 x = Vector_Mul(n[0],a1);
    sr::vec3 u = Vector_Mul(n[1],a2);
    sr::vec3 v = Vector_Mul(n[2],a3);
    sr::vec3 y = Vector_Add(u,v);

    normal = Vector_Add(x,y);

    return normal;
}

sr::vec2_i sr::Interpolate_TextureCoords(sr::vec2_i p[3], sr::vec2_i t[3], sr::vec2_i fp){
    sr::vec2_i texture_c;

    float area = Triangle_Area_i(p);
    sr::vec2_i a1v[3] = (sr::vec2_i[3]){fp,p[1],p[2]};
    sr::vec2_i a2v[3] = (sr::vec2_i[3]){p[0],fp,p[2]};
    sr::vec2_i a3v[3] = (sr::vec2_i[3]){p[0],p[1],fp};
    float a1 = Triangle_Area_i(a1v)/area;
    float a2 = Triangle_Area_i(a2v)/area;
    float a3 = Triangle_Area_i(a3v)/area;

    sr::vec2_i x = Vector_Mul_i(t[0],a1);
    sr::vec2_i u = Vector_Mul_i(t[1],a2);
    sr::vec2_i v = Vector_Mul_i(t[2],a3);
    sr::vec2_i y = Vector_Add_i(u,v);

    texture_c = Vector_Add_i(x,y);

    return texture_c;
}

sr::color sr::Interpolate_Color(sr::vec2_i p[3], sr::color c[3], sr::vec2_i fp){
    sr::color color;

    float area = Triangle_Area_i(p);
    sr::vec2_i a1v[3] = (sr::vec2_i[3]){fp,p[1],p[2]};
    sr::vec2_i a2v[3] = (sr::vec2_i[3]){p[0],fp,p[2]};
    sr::vec2_i a3v[3] = (sr::vec2_i[3]){p[0],p[1],fp};
    float a1 = Triangle_Area_i(a1v)/area;
    float a2 = Triangle_Area_i(a2v)/area;
    float a3 = Triangle_Area_i(a3v)/area;

    sr::color x = sr::color(c[0].r * a1, c[0].g * a1, c[0].b * a1);
    sr::color u = sr::color(c[1].r * a1, c[1].g * a1, c[1].b * a1);
    sr::color v = sr::color(c[2].r * a1, c[2].g * a1, c[2].b * a1);
    sr::color y = sr::color(u.r + v.r, u.g + v.g, u.b + v.b);

    color = sr::color(x.r + y.r, x.g + y.g, x.b + y.b);

    return color;
}

int sr::Triangle_ClipAgainstPlane(sr::vec3 plane_p, sr::vec3 plane_n, sr::triangle &in_tri, sr::triangle &out_tri1, sr::triangle &out_tri2)
{
    plane_n = Vector_Normalise(plane_n);

    sr::vec3* inside_points[3][2];  int nInsidePointCount = 0;
    sr::vec3* outside_points[3][2]; int nOutsidePointCount = 0;
    sr::vec3* inside_tex[3]; int nInsideTexCount = 0;
    sr::vec3* outside_tex[3]; int nOutsideTexCount = 0;


    float d0 = dist(in_tri.p[0], plane_n, plane_p);
    float d1 = dist(in_tri.p[1], plane_n, plane_p);
    float d2 = dist(in_tri.p[2], plane_n, plane_p);

    if (d0 >= 0) { 
        inside_points[nInsidePointCount][0] = &in_tri.p[0]; 
        inside_points[nInsidePointCount++][1] = &in_tri.n[0]; 
        inside_tex[nInsideTexCount++] = &in_tri.t[0]; 
    }
    else {
        outside_points[nOutsidePointCount][0] = &in_tri.p[0]; 
        outside_points[nOutsidePointCount++][1] = &in_tri.n[0]; 
        outside_tex[nOutsideTexCount++] = &in_tri.t[0];
    }
    if (d1 >= 0) {
        inside_points[nInsidePointCount][0] = &in_tri.p[1]; 
        inside_points[nInsidePointCount++][1] = &in_tri.n[1]; 
        inside_tex[nInsideTexCount++] = &in_tri.t[1];
    }
    else {
        outside_points[nOutsidePointCount][0] = &in_tri.p[1];  
        outside_points[nOutsidePointCount++][1] = &in_tri.n[1];  
        outside_tex[nOutsideTexCount++] = &in_tri.t[1];
    }
    if (d2 >= 0) {
        inside_points[nInsidePointCount][0] = &in_tri.p[2]; 
        inside_points[nInsidePointCount++][1] = &in_tri.n[2]; 
        inside_tex[nInsideTexCount++] = &in_tri.t[2];
    }
    else {
        outside_points[nOutsidePointCount][0] = &in_tri.p[2];  
        outside_points[nOutsidePointCount++][1] = &in_tri.n[2];  
        outside_tex[nOutsideTexCount++] = &in_tri.t[2];
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

        out_tri1.p[0] = *inside_points[0][0];
        out_tri1.t[0] = *inside_tex[0];

        float t;
        out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0][0], *outside_points[0][0], t);
        out_tri1.t[1].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
        out_tri1.t[1].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
        out_tri1.t[1].z = t * (outside_tex[0]->z - inside_tex[0]->z) + inside_tex[0]->z;

        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0][0], *outside_points[1][0], t);
        out_tri1.t[2].x = t * (outside_tex[1]->x - inside_tex[0]->x) + inside_tex[0]->x;
        out_tri1.t[2].y = t * (outside_tex[1]->y - inside_tex[0]->y) + inside_tex[0]->y;
        out_tri1.t[2].z = t * (outside_tex[1]->z - inside_tex[0]->z) + inside_tex[0]->z;

        sr::vec3 n_p[3] = (sr::vec3[3]){*inside_points[0][0], *outside_points[0][0], *outside_points[1][0]};
        sr::vec3 n_n[3] = (sr::vec3[3]){*inside_points[0][1], *outside_points[0][1], *outside_points[1][1]};

        out_tri1.n[0] = *inside_points[0][1];
        out_tri1.n[1] = Interpolate_Normal(n_p, n_n, out_tri1.p[1]);
        out_tri1.n[2] = Interpolate_Normal(n_p, n_n, out_tri1.p[2]);

        return 1;
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        out_tri1.c =  in_tri.c;

        out_tri2.c =  in_tri.c;

        out_tri1.p[0] = *inside_points[0][0];
        out_tri1.p[1] = *inside_points[1][0];
        out_tri1.t[0] = *inside_tex[0];
        out_tri1.t[1] = *inside_tex[1];

        float t;
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0][0], *outside_points[0][0], t);
        out_tri1.t[2].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
        out_tri1.t[2].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
        out_tri1.t[2].z = t * (outside_tex[0]->z - inside_tex[0]->z) + inside_tex[0]->z;

        out_tri2.p[0] = *inside_points[1][0];
        out_tri2.t[0] = *inside_tex[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.t[1] = out_tri1.t[2];


        out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1][0], *outside_points[0][0], t);
        out_tri2.t[2].x = t * (outside_tex[0]->x - inside_tex[1]->x) + inside_tex[1]->x;
        out_tri2.t[2].y = t * (outside_tex[0]->y - inside_tex[1]->y) + inside_tex[1]->y;
        out_tri2.t[2].z = t * (outside_tex[0]->z - inside_tex[1]->z) + inside_tex[1]->z;

        sr::vec3 n_p1[3] = (sr::vec3[3]){*inside_points[0][0], *inside_points[1][0], *outside_points[0][0]};
        sr::vec3 n_n1[3] = (sr::vec3[3]){*inside_points[0][1], *inside_points[1][1], *outside_points[0][1]};

        sr::vec3 n_p2[3] = (sr::vec3[3]){*inside_points[1][0], out_tri1.p[2], *outside_points[0][0]};
        sr::vec3 n_n2[3] = (sr::vec3[3]){*inside_points[1][1], out_tri1.n[2], *outside_points[0][1]};

        out_tri1.n[0] = *inside_points[0][1];
        out_tri1.n[1] = *inside_points[1][1];
        out_tri1.n[2] = Interpolate_Normal(n_p1, n_n1, out_tri1.p[2]);

        out_tri2.n[0] = *inside_points[1][1];
        out_tri2.n[1] = out_tri1.n[2];
        out_tri2.n[2] = Interpolate_Normal(n_p2, n_n2, out_tri2.p[2]);

        return 2;
    }
    return 0;
}

//-----------------------------------------------------------------------------
// [SECTION] Matrix
//-----------------------------------------------------------------------------

sr::vec3 sr::Vector_MultiplyMatrix(sr::vec3 &i, sr::mat4x4 &m)
{
    sr::vec3 v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    return v;
}


sr::mat4x4 sr::Matrix_MultiplyMatrix(sr::mat4x4 &m1, sr::mat4x4 &m2)
{
    sr::mat4x4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
}


sr::mat4x4 sr::Matrix_MakeIdentity()
{
    sr::mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}


sr::mat4x4 sr::Matrix_Projection(int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float Zfar, float Znear)
{
    sr::mat4x4 matrix;
    matrix.m[0][0] = (SCREEN_HEIGHT/SCREEN_HEIGHT) * (1/tan(degToRad(FOV/2)));
    matrix.m[1][1] = (1/tan(degToRad(FOV/2)));
    matrix.m[2][2] = Zfar / (Zfar-Znear);
    matrix.m[3][2] = (-Zfar * Znear) / (Zfar - Znear);
    matrix.m[2][3] = 1;
    matrix.m[3][3] = 0;
    return matrix;
}


sr::mat4x4 sr::Translate(sr::vec3 v2){
    sr::mat4x4 matrix;

    matrix.m[0][0] = 1;
    matrix.m[1][1] = 1;
    matrix.m[2][2] = 1;
    matrix.m[3][3] = 1;
    matrix.m[3][0] = v2.x;
    matrix.m[3][1] = v2.y;
    matrix.m[3][2] = v2.z;

    return matrix;
}


sr::mat4x4 sr::Scale(sr::vec3 v2){
    sr::mat4x4 matrix;

    matrix.m[0][0] = v2.x;
    matrix.m[1][1] = v2.y;
    matrix.m[2][2] = v2.z;
    matrix.m[3][3] = 1;

    return matrix;
}


sr::mat4x4 sr::Matrix_RotateX(float ang)
{
    float fAngleRad = degToRad(ang);
    sr::mat4x4 matrix;
    matrix.m[0][0] = 1;
    matrix.m[1][1] = cos(fAngleRad);
    matrix.m[1][2] = sin(fAngleRad);
    matrix.m[2][1] = -sin(fAngleRad);
    matrix.m[2][2] = cos(fAngleRad);
    matrix.m[3][3] = 1;
    return matrix;
}


sr::mat4x4 sr::Matrix_RotateY(float ang)
{
    sr::mat4x4 matrix;
    matrix.m[0][0] = cos(degToRad(ang));
    matrix.m[1][1] = 1;
    matrix.m[0][2] = -sin(degToRad(ang));
    matrix.m[2][0] = sin(degToRad(ang));
    matrix.m[2][2] = cos(degToRad(ang));
    matrix.m[3][3] = 1;
    return matrix;
}


sr::mat4x4 sr::Matrix_RotateZ(float ang)
{
    float fAngleRad = degToRad(ang);
    sr::mat4x4 matrix;
    matrix.m[0][0] = cos(fAngleRad);
    matrix.m[0][1] = sin(fAngleRad);
    matrix.m[1][0] = -sin(fAngleRad);
    matrix.m[1][1] = cos(fAngleRad);
    matrix.m[2][2] = 1;
    matrix.m[3][3] = 1;
    return matrix;
}


sr::mat4x4 sr::Matrix_CameraPos(sr::player_t &pos, sr::vec3 &target, sr::vec3 &up){

    sr::vec3 newForward; 
    newForward.x = target.x-pos.x;
    newForward.y = target.y-pos.y;
    newForward.z = target.z-pos.z;
    float NFl = sqrt(newForward.x*newForward.x + newForward.y*newForward.y + newForward.z*newForward.z);
    newForward.x /= NFl; newForward.y /= NFl; newForward.z /= NFl;

    float NFUdp = up.x*newForward.x+up.y*newForward.y+up.z*newForward.z;
    sr::vec3 a; 
    a.x = newForward.x*NFUdp;
    a.y = newForward.y*NFUdp;
    a.z = newForward.z*NFUdp;
    sr::vec3 newUp; 
    newUp.x = up.x-a.x;
    newUp.y = up.y-a.y;
    newUp.z = up.z-a.z;
    float NUl = sqrt(newUp.x*newUp.x + newUp.y*newUp.y + newUp.z*newUp.z);
    newUp.x /= NUl; newUp.y /= NUl; newUp.z /= NUl;

    sr::vec3 newRight; 
    newRight.x = newUp.y * newForward.z - newUp.z * newForward.y;
    newRight.y = newUp.z * newForward.x - newUp.x * newForward.z;
    newRight.z = newUp.x * newForward.y - newUp.y * newForward.x;

    sr::mat4x4 matrix;
    matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0;
    matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0;
    matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0;
    matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1;
    return matrix;

}


sr::mat4x4 sr::Matrix_QuickInverse(sr::mat4x4 &m)
{
    sr::mat4x4 matrix;
    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1;
    return matrix;
}

//-----------------------------------------------------------------------------
// [SECTION] Rasterization
//-----------------------------------------------------------------------------


void sr::bresanham(int x1, int y1, int x2, int y2, std::function<void(int, int)> func)
{
    
    float dx = x2-x1;
    float dy = y2-y1;

    // Find Signs
    int sx = (dx>=0) ? 1 : (-1);
    int sy = (dy>=0) ? 1 : (-1);

    // Get Initial Points
    float x = x1;
    float y = y1;

    // Flag to check if swapping happens
    int isSwaped = 0;

    // Swap if needed
    if(abs(dy) > abs(dx))
    {
        // swap dx and dy
        float tdx = dx;
        dx =dy;
        dy = tdx;

        isSwaped = 1;
    }

    // Decision parameter
    float p = 2*(abs(dy)) - abs(dx);

    //Print Initial Point
    func(x,y);

    // Loop for dx times
    for(int i = 0; i<= abs(dx);i++)
    {
        // Depending on decision parameter
        if(p < 0)
        {
            if(isSwaped == 0){
                x = x + sx;
                func(x,y);
            }
            else{
                y = y+sy;
                func(x,y);
            }
            p = p + 2*abs(dy);
        }
        else
        {
            x = x+sx;
            y = y+sy;
            func(x,y);
            p = p + 2*abs(dy) - 2*abs(dx);
        }
    }
}

void sr::Raster(sr::vec2_i p[3], std::function<void(sr::vec2_i)> pixel, int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT){

    int y_min_l = SCREEN_HEIGHT-1, y_max_l = 0;
    int *y_min = &y_min_l, *y_max = &y_max_l;

    p[0].y = (p[0].y < 1 ? 2 : (p[0].y > SCREEN_HEIGHT-1 ? SCREEN_HEIGHT-2 : p[0].y));
    p[1].y = (p[1].y < 1 ? 2 : (p[1].y > SCREEN_HEIGHT-1 ? SCREEN_HEIGHT-2 : p[1].y));
    p[2].y = (p[2].y < 1 ? 2 : (p[2].y > SCREEN_HEIGHT-1 ? SCREEN_HEIGHT-2 : p[2].y));

    std::function<void(int, int)> f = [&](int x, int y)
    {
        if(x < scanline_buffer[y])
            scanline_buffer[y] = x;
        if(x > scanline_buffer[SCREEN_HEIGHT + y])
            scanline_buffer[SCREEN_HEIGHT + y] = x;

        if(y > *y_max)
            *y_max = y;
        if(y < *y_min)
            *y_min = y;
    };

    bresanham(p[0].x, p[0].y, p[1].x, p[1].y, f);
    bresanham(p[1].x, p[1].y, p[2].x, p[2].y, f);
    bresanham(p[2].x, p[2].y, p[0].x, p[0].y, f);

    for(int y = *y_min; y <= *y_max; y++){
        for(int i = scanline_buffer[y]; i <= scanline_buffer[SCREEN_HEIGHT + y]; i++){
            pixel(sr::vec2_i(i, y));
        }
        scanline_buffer[y] = SCREEN_WIDTH-1;
        scanline_buffer[SCREEN_HEIGHT + y] = 0;
    }
}

void sr::Raster_NonInterpolated(SDL_Renderer *renderer, sr::vec2_i p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, sr::color color){
    std::function<void(sr::vec2_i pixel)> f = [&](sr::vec2_i pixel)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawPoint(renderer, (int)pixel.x, (int)pixel.y);
    };

    Raster(p, f, scanline_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
}
void sr::Raster_ColorInterpolated(SDL_Renderer *renderer, sr::vec2_i p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, sr::color colors[3]){
    std::function<void(sr::vec2_i pixel)> f = [&](sr::vec2_i pixel)
    {
        sr::color color = Interpolate_Color(p, colors, pixel);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawPoint(renderer, (int)pixel.x, (int)pixel.y);
    };

    Raster(p, f, scanline_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
}
void sr::Raster_NormalInterpolated(SDL_Renderer *renderer, sr::vec2_i p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, sr::color color, sr::vec3 normals[3], sr::vec3 light){
    std::function<void(sr::vec2_i pixel)> f = [&](sr::vec2_i pixel)
    {
        sr::vec3 normal = Interpolate_Normal_i(p, normals, pixel);
        normal = Vector_Normalise(normal);
        float light_dp = Vector_DotProduct(normal, light);
        const uint8_t min_light_value = 10;
        float light_value = (float)(((light_dp + 1)/2)*(255 - min_light_value) + min_light_value)/255;
        sr::color shade = sr::color(
            (uint8_t)((float)color.r * light_value),
            (uint8_t)((float)color.g * light_value),
            (uint8_t)((float)color.b * light_value)
        );
        SDL_SetRenderDrawColor(renderer, shade.r, shade.g, shade.b, 255);
        SDL_RenderDrawPoint(renderer, (int)pixel.x, (int)pixel.y);
    };

    Raster(p, f, scanline_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
}
void sr::Raster_ColorAndNormalInterpolated(SDL_Renderer *renderer, sr::vec2_i p[3], int *scanline_buffer, int SCREEN_WIDTH, int SCREEN_HEIGHT, sr::color colors[3], sr::vec3 normals[3], sr::vec3 light){
    std::function<void(sr::vec2_i pixel)> f = [&](sr::vec2_i pixel)
    {
        sr::vec3 normal = Interpolate_Normal_i(p, normals, pixel);
        normal = Vector_Normalise(normal);
        sr::color color = Interpolate_Color(p, colors, pixel);
        float light_dp = Vector_DotProduct(normal, light);
        const uint8_t min_light_value = 10;
        float light_value = (float)(((light_dp + 1)/2)*(255 - min_light_value) + min_light_value)/255;
        sr::color shade = sr::color(
            (uint8_t)((float)color.r * light_value),
            (uint8_t)((float)color.g * light_value),
            (uint8_t)((float)color.b * light_value)
        );
        SDL_SetRenderDrawColor(renderer, shade.r, shade.g, shade.b, 255);
        SDL_RenderDrawPoint(renderer, (int)pixel.x, (int)pixel.y);
    };

    Raster(p, f, scanline_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
}


//-----------------------------------------------------------------------------
// [SECTION] Rendering
//-----------------------------------------------------------------------------


void sr::Frame(SDL_Renderer *renderer, std::vector<sr::mesh> mesh_collection, sr::mat4x4 Projection_Matrix, sr::player_t Camera, float *pDepthBuffer, sr::vec3 light, int SCREEN_WIDTH, int SCREEN_HEIGHT, bool phong){

    int *scanline_buffer = new int[SCREEN_HEIGHT * 2];

    for(int y = 0; y < SCREEN_HEIGHT; y++){
        scanline_buffer[y] = SCREEN_WIDTH-1;
        scanline_buffer[SCREEN_HEIGHT + y] = 0;
    }

    std::fill_n(pDepthBuffer, SCREEN_HEIGHT*SCREEN_WIDTH, 0.0f);

    float l_length = sqrt(light.x*light.x + light.y*light.y + light.z*light.z);
    light.x /= l_length; light.y /= l_length; light.z /= l_length;

    sr::vec3 vLookDir;
    vLookDir.z = 1;
    sr::mat4x4 LookRx, LookRy;

    LookRx = Matrix_RotateX(-Camera.angy);
    LookRy = Matrix_RotateY(Camera.angx);
    LookRy = Matrix_MultiplyMatrix(LookRx, LookRy);
    vLookDir = Vector_MultiplyMatrix(vLookDir, LookRy);

    sr::vec3 pc1;
    sr::vec3 pc2;
    sr::vec3 pc3;
    sr::vec3 pc4;
    sr::vec3 pc5;
    sr::vec3 pc6;
    sr::vec3 pc7;
    pc2.y = 1;
    pc3.y = (float)SCREEN_HEIGHT - 1;
    pc4.y = -1;
    pc5.x = 1;
    pc6.x = (float)SCREEN_WIDTH - 1;
    pc7.x = -1;

    

    sr::vec3 vUp;
    vUp.y = 1;
    sr::vec3 vTarget; 
    vTarget.x = Camera.x + vLookDir.x;
    vTarget.y = Camera.y + vLookDir.y;
    vTarget.z = Camera.z + vLookDir.z;

    sr::mat4x4 matCamera = Matrix_CameraPos(Camera, vTarget, vUp);
    sr::mat4x4 matView = Matrix_QuickInverse(matCamera);

    for(int obj = 0; obj < mesh_collection.size(); obj++){
        sr::mesh mesh = mesh_collection[obj];

        sr::mat4x4 RotX, RotY, RotZ, Size, Move, mesh_Matrix, normal_Matrix;

        RotX = Matrix_RotateX(mesh.rotation.x+180);
        RotY = Matrix_RotateY(mesh.rotation.y);
        RotZ = Matrix_RotateZ(mesh.rotation.z);
        Size = Scale(mesh.size);
        Move = Translate(mesh.position);

        normal_Matrix = Matrix_MultiplyMatrix(RotZ, RotY);
        normal_Matrix = Matrix_MultiplyMatrix(normal_Matrix, RotX);

        mesh_Matrix = Matrix_MultiplyMatrix(Size, RotZ);
        mesh_Matrix = Matrix_MultiplyMatrix(mesh_Matrix, RotY);
        mesh_Matrix = Matrix_MultiplyMatrix(mesh_Matrix, RotX);
        mesh_Matrix = Matrix_MultiplyMatrix(mesh_Matrix, Move);

        for(int i = 0; i < mesh.tris.size(); i++){
            if(!phong){
                sr::triangle transformation, projection, view;
                sr::vec3 normal, line1, line2;

                transformation = mesh.tris[i];

                for(int j = 0; j < 3; j++){
                    transformation.p[j] = Vector_MultiplyMatrix(transformation.p[j], mesh_Matrix);
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
                    sr::triangle clipped[2];
                    sr::vec3 p_1;
                    sr::vec3 p_2;
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

                        projection.c = (sr::color){shade, shade, shade};




                        sr::triangle clipped[2];
                        std::list<sr::triangle> listTriangles;

                        listTriangles.push_back(projection);
                        int nNewTriangles = 1;

                        for (int p = 0; p < 4; p++)
                        {
                            int nTrisToAdd = 0;
                            while (nNewTriangles > 0)
                            {
                                sr::triangle test = listTriangles.front();
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


                        for (std::list<sr::triangle>::iterator t = listTriangles.begin(); t != listTriangles.end(); t++)
                        {
                            sr::vec2_i p[3] = (sr::vec2_i[3]){
                                sr::vec2_i((int)roundf(t->p[0].x),(int)roundf(t->p[0].y)),
                                sr::vec2_i((int)roundf(t->p[1].x),(int)roundf(t->p[1].y)),
                                sr::vec2_i((int)roundf(t->p[2].x),(int)roundf(t->p[2].y))
                            };

                            if(!mesh.texture){
                                sr::color color = sr::color( LZ(t->c.r + mesh.color.r - 255), LZ(t->c.g + mesh.color.g - 255), LZ(t->c.b + mesh.color.b - 255));
                                Raster_NonInterpolated(renderer, p, scanline_buffer, SCREEN_WIDTH, SCREEN_HEIGHT, color);
                            }else{
                                const std::vector< SDL_Vertex > verts =
                                {
                                    { SDL_FPoint{ t->p[0].x, t->p[0].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[0].x, t->t[0].y }, },
                                    { SDL_FPoint{ t->p[1].x, t->p[1].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[1].x, t->t[1].y }, },
                                    { SDL_FPoint{ t->p[2].x, t->p[2].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[2].x, t->t[2].y }, },
                                };
                                SDL_RenderGeometry( renderer, mesh.texture, verts.data(), verts.size(), nullptr, 0 );
                            }
                        }
                    }
                }
            }else {
                sr::triangle transformation, projection, view;
                sr::vec3 normal, line1, line2;

                transformation = mesh.tris[i];

                for(int j = 0; j < 3; j++){
                    transformation.p[j] = Vector_MultiplyMatrix(transformation.p[j], mesh_Matrix);
                    transformation.n[j] = Vector_MultiplyMatrix(transformation.n[j], normal_Matrix);
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

                    

                    for(int j = 0; j < 3; j++){
                        view.p[j] = Vector_MultiplyMatrix(transformation.p[j], matView);
                        view.t[j] = transformation.t[j];
                        view.n[j] = Vector_MultiplyMatrix(transformation.n[j], matView);
                    }


                    int nClippedTriangles = 0;
                    sr::triangle clipped[2];
                    sr::vec3 p_1;
                    sr::vec3 p_2;
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

                        projection.n[0] = clipped[n].n[0];
                        projection.n[1] = clipped[n].n[1];
                        projection.n[2] = clipped[n].n[2];
                        

                        sr::triangle clipped[2];
                        std::list<sr::triangle> listTriangles;

                        listTriangles.push_back(projection);
                        int nNewTriangles = 1;

                        for (int p = 0; p < 4; p++)
                        {
                            int nTrisToAdd = 0;
                            while (nNewTriangles > 0)
                            {
                                sr::triangle test = listTriangles.front();
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


                        for (std::list<sr::triangle>::iterator t = listTriangles.begin(); t != listTriangles.end(); t++)
                        {
                            sr::vec2_i p[3] = (sr::vec2_i[3]){
                                sr::vec2_i((int)floorf(t->p[0].x),(int)floorf(t->p[0].y)),
                                sr::vec2_i((int)floorf(t->p[1].x),(int)floorf(t->p[1].y)),
                                sr::vec2_i((int)floorf(t->p[2].x),(int)floorf(t->p[2].y))
                            };

                            t->n[0] = Vector_Normalise(t->n[0]);
                            t->n[1] = Vector_Normalise(t->n[1]);
                            t->n[2] = Vector_Normalise(t->n[2]);

                            if(!mesh.texture){
                                Raster_NormalInterpolated(renderer, p, scanline_buffer, SCREEN_WIDTH, SCREEN_HEIGHT, mesh.color, t->n, light);
                            }else{
                                const std::vector< SDL_Vertex > verts =
                                {
                                    { SDL_FPoint{ t->p[0].x, t->p[0].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[0].x, t->t[0].y }, },
                                    { SDL_FPoint{ t->p[1].x, t->p[1].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[1].x, t->t[1].y }, },
                                    { SDL_FPoint{ t->p[2].x, t->p[2].y }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ t->t[2].x, t->t[2].y }, },
                                };
                                SDL_RenderGeometry( renderer, mesh.texture, verts.data(), verts.size(), nullptr, 0 );
                            }
                        }
                    }
                }
            }
        }
    }
}