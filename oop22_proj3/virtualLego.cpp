////////////////////////////////////////////////////////////////////////////////
//
// File: virtualLego.cpp
//
// Original Author: 박창현 Chang-hyeon Park, 
// Modified by Bong-Soo Sohn and Dong-Jun Kim
// 
// Originally programmed for Virtual LEGO. 
// Modified later to program for Virtual Billiard.
//        
////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>

IDirect3DDevice9* Device = NULL;

// window size
const int Width = 1024;
const int Height = 768;

// 0.42
// There are four balls
// initialize the position (coordinate) of each ball (ball0 ~ ball3)
// const float spherePos[4][2] = { {-2.7f,0} , {+2.4f,0} , {3.3f,0} , {-2.7f,-0.9f} };
const float spherePos[54][2] = { {-0.21f,-2.0f} , {-0.63f, -2.0f} , {0.63f, -2.0f} , {0.21f, -2.0f},
								{-1.05f, -2.0f}, {-1.47f, -2.0f}, {1.47f, -2.0f}, {1.05f, -2.0f},
								{-1.8f, -1.7f}, {-2.1f, -1.4f}, {2.1f, -1.4f}, {1.8f, -1.7f},
								{-2.1f, -0.98f}, {-2.1f, -0.56f}, {2.1f, -0.56f}, {2.1f, -0.98f},
								{-2.1f, -0.14f}, {-2.1f, 0.28f}, {2.1f, 0.28f}, {2.1f, -0.14f},
								{-2.1f, 0.7f}, {-2.1f, 1.12f}, {2.1f, 1.12f}, {2.1f, 0.7f},
								{-2.1f, 1.54f}, {-2.1f, 1.96f}, {2.1f, 1.96f}, {2.1f, 1.54f},
								{-2.1f, 2.38f}, {-1.8f, 2.68f}, {1.8f, 2.68f}, {2.1f, 2.38f},
								{-1.47f, 2.98f}, {-1.05f, 2.98f}, {1.05f, 2.98f}, {1.47f, 2.98f},
								{-0.63f, 2.87f}, {-0.21f, 2.98f}, {0.21f, 2.98f}, {0.63f, 2.98f},
								{-0.21f, -0.56f}, {-0.63f, -0.56f}, {0.63f, -0.56f}, {0.21f, -0.56f},
								{-0.93f, -0.26f}, {-1.23f, 0.04f}, {1.23f, 0.04f}, {0.93f, -0.26f},
								{-0.21f, 0.28f}, {-0.21f, 0.7f}, {-0.93f, 1.54f}, {-0.93f, 1.54f},
								{0.93f, 1.96f}, {0.93f, 1.96f} };

const D3DXCOLOR sphereColor = d3d::YELLOW;

// -----------------------------------------------------------------------------
// Transform matrices
// -----------------------------------------------------------------------------

D3DXMATRIX g_mWorld;
D3DXMATRIX g_mView;
D3DXMATRIX g_mProj;

#define M_RADIUS 0.21   // ball radius
#define PI 3.14159265
#define M_HEIGHT 0.01
#define DECREASE_RATE 0.9982

// -----------------------------------------------------------------------------
// CSphere class definition
// -----------------------------------------------------------------------------

class CSphere {
private:
	float					center_x, center_y, center_z;
	float                   m_radius;
	float					m_velocity_x;
	float					m_velocity_z;

public:
	CSphere(void)
	{
		D3DXMatrixIdentity(&m_mLocal);
		ZeroMemory(&m_mtrl, sizeof(m_mtrl));
		m_radius = 0;
		m_velocity_x = 0;
		m_velocity_z = 0;
		m_pSphereMesh = NULL;
	}
	~CSphere(void) {}

public:
	bool create(IDirect3DDevice9* pDevice, D3DXCOLOR color = d3d::WHITE)
	{
		if (NULL == pDevice)
			return false;

		m_mtrl.Ambient = color;
		m_mtrl.Diffuse = color;
		m_mtrl.Specular = color;
		m_mtrl.Emissive = d3d::BLACK;
		m_mtrl.Power = 5.0f;

		if (FAILED(D3DXCreateSphere(pDevice, getRadius(), 50, 50, &m_pSphereMesh, NULL)))
			return false;
		return true;
	}

	void destroy(void)
	{
		if (m_pSphereMesh != NULL) {
			m_pSphereMesh->Release();
			m_pSphereMesh = NULL;
		}
	}

	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
	{
		if (NULL == pDevice)
			return;
		pDevice->SetTransform(D3DTS_WORLD, &mWorld);
		pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
		pDevice->SetMaterial(&m_mtrl);
		m_pSphereMesh->DrawSubset(0);
	}

	bool hasIntersected(CSphere& ball)
	{
		// Insert your code here.

		return false;
	}

	/*
	void hitBy(CSphere& ball) // ball is shotPos
	{
		D3DXVECTOR3 hitPos = this->getCenter();
		D3DXVECTOR3	shotPos = ball.getCenter();
		double dist = sqrt(pow(shotPos.x - hitPos.x, 2) + pow(shotPos.z - hitPos.z, 2));

		float vx = ball.getVelocity_X(); float vz = ball.getVelocity_Z();
		float shotTan = vz / vx;

		float dx = hitPos.x - shotPos.x; float dz = hitPos.z - shotPos.z;
		float collideTan = dz / dx;

		float limitTan = -dz / dx;

		if (dist < 0.42) {
			if (dx >= 0 && dz >= 0) {
				if (shotTan > collideTan || shotTan < limitTan) { 
					ball.setPower(-dz / dist, dx / dist);
				}
				else if (shotTan < collideTan && shotTan > limitTan) {
					ball.setPower(dz / dist, -dx / dist);
				}
			}
			else if (dx < 0 && dz >= 0) {
				if (shotTan > collideTan || shotTan < limitTan) {
					ball.setPower(dz / dist, -dx / dist);
				}
				else if (shotTan < collideTan && shotTan > limitTan) {
					ball.setPower(-dz / dist, dx / dist);
				}
			}
			else if (dx >= 0 && dz < 0) {
				if (shotTan > collideTan || shotTan < limitTan) {
					ball.setPower(dz / dist, -dx / dist);
				}
				else if (shotTan < collideTan && shotTan > limitTan) {
					ball.setPower(-dz / dist, dx / dist);
				}
			}
			else {
				if (shotTan > collideTan || shotTan < limitTan) {
					ball.setPower(-dz / dist, dx / dist);
				}
				else if (shotTan < collideTan && shotTan > limitTan) {
					ball.setPower(dz / dist, -dx / dist);
				}
			}
			this->destroy();
		}
	}
	*/
	void hitBy(CSphere& ball) // ball은 shotPos
	{
		D3DXVECTOR3 hitPos = this->getCenter();
		D3DXVECTOR3	shotPos = ball.getCenter();
		double dist = sqrt(pow(shotPos.x - hitPos.x, 2) + pow(shotPos.z - hitPos.z, 2));

		float vx = ball.getVelocity_X(); float vz = ball.getVelocity_Z();

		float dx = hitPos.x - shotPos.x; float dz = hitPos.z - shotPos.z;

		if (dist < 0.42) {
			if (dx == 0) ball.setPower(0, 2);
			else if (dx != 0) {
				float shotTan;
				if (vx == 0) shotTan = pow(10, 8);
				else shotTan = vz / vx;

				float collideTan = dz / dx;
				double dbRadian = PI / 2 - (atan(collideTan) - atan(shotTan));
				double dbDegree = floor((180 / PI) * dbRadian);
				double newTan = tan(dbDegree);
				double v = 2 / sqrt(1 + pow(newTan, 2));
				ball.setPower(v, v * newTan);
			}
			this->destroy();
		}
	}

	void ballUpdate(float timeDiff)
	{
		//const float TIME_SCALE = 3.3;
		D3DXVECTOR3 cord = this->getCenter();
		double vx = abs(this->getVelocity_X());
		double vz = abs(this->getVelocity_Z());

		if (vx > 0.01 || vz > 0.01)
		{
			float tX = cord.x +  timeDiff * m_velocity_x;
			float tZ = cord.z +  timeDiff * m_velocity_z;

			//correction of position of ball
			// Please uncomment this part because this correction of ball position is necessary when a ball collides with a wall
			if (tX >= (3 - M_RADIUS)) {
				tX = 3 - M_RADIUS;
				this->setPower(-getVelocity_X(), getVelocity_Z());
			}
			else if(tX <=(-3 + M_RADIUS)){
				tX = -3 + M_RADIUS;
				this->setPower(-getVelocity_X(), getVelocity_Z());
			}
			else if (tZ >= (4.5 - M_RADIUS)) {
				tZ = 4.5 - M_RADIUS;
				this->setPower(getVelocity_X(), -getVelocity_Z());
			}
			
			this->setCenter(tX, cord.y, tZ);
		}
		else { this->setPower(0, 0); }

		this->setPower(getVelocity_X(), getVelocity_Z());
	}

	double getVelocity_X() { return this->m_velocity_x; }
	double getVelocity_Z() { return this->m_velocity_z; }

	void setPower(double vx, double vz)
	{
		this->m_velocity_x = vx;
		this->m_velocity_z = vz;
	}

	void setCenter(float x, float y, float z)
	{
		D3DXMATRIX m;
		center_x = x;	center_y = y;	center_z = z;
		D3DXMatrixTranslation(&m, x, y, z);
		setLocalTransform(m);
	}

	float getRadius(void)  const { return (float)(M_RADIUS); }
	const D3DXMATRIX& getLocalTransform(void) const { return m_mLocal; }
	void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
	D3DXVECTOR3 getCenter(void) const
	{
		D3DXVECTOR3 org(center_x, center_y, center_z);
		return org;
	}

	bool isNull(void) {
		if (m_pSphereMesh) {
			return false;
		}
		else {
			return true;
		}
	}
private:
	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pSphereMesh;

};



class CHolderSphere : public CSphere{

public:
	void hitBy(CSphere &ball, bool isShot) // ball은 shotPos
	{ 
		D3DXVECTOR3 hitPos = this->getCenter();
		D3DXVECTOR3	shotPos = ball.getCenter();
		double dist = sqrt(pow(shotPos.x - hitPos.x, 2) + pow(shotPos.z - hitPos.z, 2));

		float vx = ball.getVelocity_X(); float vz = ball.getVelocity_Z();

		float dx = hitPos.x - shotPos.x; float dz = hitPos.z - shotPos.z;
		
		if (dist < 0.42 && isShot) {
			if (vx == 0) float shotTan = pow(10, 8);
			if (dx == 0) ball.setPower(1, 1);
			else if (dx != 0) {
				float shotTan;
				if (vx == 0) shotTan = pow(10, 8);
				else shotTan = vz / vx;

				float collideTan = dz / dx;
				double dbRadian = PI / 2 - (atan(collideTan) - atan(shotTan));
				double dbDegree = floor((180 / PI) * dbRadian);
				double newTan = tan(dbDegree);
				double v = 2 / sqrt(1 + pow(newTan, 2));
				ball.setPower(v, v * newTan);
			}
		}
	}
};

// -----------------------------------------------------------------------------
// CWall class definition
// -----------------------------------------------------------------------------

class CWall {

private:

	float					m_x;
	float					m_z;
	float                   m_width;
	float                   m_depth;
	float					m_height;

public:
	CWall(void)
	{
		D3DXMatrixIdentity(&m_mLocal);
		ZeroMemory(&m_mtrl, sizeof(m_mtrl));
		m_width = 0;
		m_depth = 0;
		m_pBoundMesh = NULL;
	}
	~CWall(void) {}
public:
	bool create(IDirect3DDevice9* pDevice, float ix, float iz, float iwidth, float iheight, float idepth, D3DXCOLOR color = d3d::WHITE)
	{
		if (NULL == pDevice)
			return false;

		m_mtrl.Ambient = color;
		m_mtrl.Diffuse = color;
		m_mtrl.Specular = color;
		m_mtrl.Emissive = d3d::BLACK;
		m_mtrl.Power = 5.0f;

		m_width = iwidth;
		m_depth = idepth;

		if (FAILED(D3DXCreateBox(pDevice, iwidth, iheight, idepth, &m_pBoundMesh, NULL)))
			return false;
		return true;
	}
	void destroy(void)
	{
		if (m_pBoundMesh != NULL) {
			m_pBoundMesh->Release();
			m_pBoundMesh = NULL;
		}
	}
	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
	{
		if (NULL == pDevice)
			return;
		pDevice->SetTransform(D3DTS_WORLD, &mWorld);
		pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
		pDevice->SetMaterial(&m_mtrl);
		m_pBoundMesh->DrawSubset(0);
	}

	bool hasIntersected(CSphere& ball)
	{
		// Insert your code here.
		return false;
	}
	
	bool hitBy(CSphere& ball)
	{
			
		D3DXVECTOR3	shotPos = ball.getCenter();
		double cur_velocity_x = ball.getVelocity_X();
		double cur_velocity_z = ball.getVelocity_Z();

		if (shotPos.z <= -8.25) {
			return true;
		}
		return false;
	}

	void setPosition(float x, float y, float z)
	{
		D3DXMATRIX m;
		this->m_x = x;
		this->m_z = z;

		D3DXMatrixTranslation(&m, x, y, z);
		setLocalTransform(m);
	}

	float getHeight(void) const { return M_HEIGHT; }



private:
	void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }

	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pBoundMesh;
};

// -----------------------------------------------------------------------------
// CLight class definition
// -----------------------------------------------------------------------------

class CLight {
public:
	CLight(void)
	{
		static DWORD i = 0;
		m_index = i++;
		D3DXMatrixIdentity(&m_mLocal);
		::ZeroMemory(&m_lit, sizeof(m_lit));
		m_pMesh = NULL;
		m_bound._center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_bound._radius = 0.0f;
	}
	~CLight(void) {}
public:
	bool create(IDirect3DDevice9* pDevice, const D3DLIGHT9& lit, float radius = 0.1f)
	{
		if (NULL == pDevice)
			return false;
		if (FAILED(D3DXCreateSphere(pDevice, radius, 10, 10, &m_pMesh, NULL)))
			return false;

		m_bound._center = lit.Position;
		m_bound._radius = radius;

		m_lit.Type = lit.Type;
		m_lit.Diffuse = lit.Diffuse;
		m_lit.Specular = lit.Specular;
		m_lit.Ambient = lit.Ambient;
		m_lit.Position = lit.Position;
		m_lit.Direction = lit.Direction;
		m_lit.Range = lit.Range;
		m_lit.Falloff = lit.Falloff;
		m_lit.Attenuation0 = lit.Attenuation0;
		m_lit.Attenuation1 = lit.Attenuation1;
		m_lit.Attenuation2 = lit.Attenuation2;
		m_lit.Theta = lit.Theta;
		m_lit.Phi = lit.Phi;
		return true;
	}
	void destroy(void)
	{
		if (m_pMesh != NULL) {
			m_pMesh->Release();
			m_pMesh = NULL;
		}
	}
	bool setLight(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
	{
		if (NULL == pDevice)
			return false;

		D3DXVECTOR3 pos(m_bound._center);
		D3DXVec3TransformCoord(&pos, &pos, &m_mLocal);
		D3DXVec3TransformCoord(&pos, &pos, &mWorld);
		m_lit.Position = pos;

		pDevice->SetLight(m_index, &m_lit);
		pDevice->LightEnable(m_index, TRUE);
		return true;
	}

	void draw(IDirect3DDevice9* pDevice)
	{
		if (NULL == pDevice)
			return;
		D3DXMATRIX m;
		D3DXMatrixTranslation(&m, m_lit.Position.x, m_lit.Position.y, m_lit.Position.z);
		pDevice->SetTransform(D3DTS_WORLD, &m);
		pDevice->SetMaterial(&d3d::WHITE_MTRL);
		m_pMesh->DrawSubset(0);
	}

	D3DXVECTOR3 getPosition(void) const { return D3DXVECTOR3(m_lit.Position); }

private:
	DWORD               m_index;
	D3DXMATRIX          m_mLocal;
	D3DLIGHT9           m_lit;
	ID3DXMesh* m_pMesh;
	d3d::BoundingSphere m_bound;
};


// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------
int		g_point;
CWall	g_legoPlane;
CWall	g_legowall[3];
CSphere	g_sphere[54];
CSphere g_shotBall;
//CSphere g_holderBall;
CHolderSphere	g_holderBall;
CLight	g_light;
bool	isShot;

double  g_camera_pos[3] = { 0.0, 10.0, -8.0 };

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------


void destroyAllLegoBlock(void)
{
}

// initialization
bool Setup()
{
	int i;

	D3DXMatrixIdentity(&g_mWorld);
	D3DXMatrixIdentity(&g_mView);
	D3DXMatrixIdentity(&g_mProj);

	// create plane and set the position
	if (false == g_legoPlane.create(Device, -1, -1, 6, 0.03f, 9, d3d::GREEN)) return false;
	g_legoPlane.setPosition(0.0f, -0.0006f / 5, 0.0f);// x, y, z가 바닥면의 위치

	// create walls and set the position. note that there are four walls
	if (false == g_legowall[0].create(Device, -1, -1, 6, 0.3f, 0.12f, d3d::BLACK)) return false;
	g_legowall[0].setPosition(0.0f, 0.12f, 4.56f); // up
	//if (false == g_legowall[1].create(Device, -1, -1, 6, 0.3f, 0.12f, d3d::BLUE)) return false;
	//g_legowall[1].setPosition(0.0f, 0.12f, -4.56f);
	if (false == g_legowall[1].create(Device, -1, -1, 0.12f, 0.3f, 9.24f, d3d::BLACK)) return false;
	g_legowall[1].setPosition(3.06f, 0.12f, 0.0f); // right
	if (false == g_legowall[2].create(Device, -1, -1, 0.12f, 0.3f, 9.24f, d3d::BLACK)) return false;
	g_legowall[2].setPosition(-3.06f, 0.12f, 0.0f); // left

	// create four balls and set the position
	for (i = 0; i < 54; i++) {
		if (false == g_sphere[i].create(Device, sphereColor)) return false;
		g_sphere[i].setCenter(spherePos[i][0], (float)M_RADIUS, spherePos[i][1]);
		g_sphere[i].setPower(0, 0);
	}

	// create white holder ball for set direction
	if (false == g_holderBall.create(Device, d3d::WHITE)) return false;
	g_holderBall.setCenter(.0f, (float)M_RADIUS, -4.3f);

	
	// create red shot ball for set direction
	if (false == g_shotBall.create(Device, d3d::RED)) return false;
	g_shotBall.setCenter(.0f, (float)M_RADIUS, -3.88f);
	
	
	// light setting 
	D3DLIGHT9 lit;
	::ZeroMemory(&lit, sizeof(lit));
	lit.Type = D3DLIGHT_POINT;
	lit.Diffuse = d3d::WHITE;
	lit.Specular = d3d::WHITE * 0.9f;
	lit.Ambient = d3d::WHITE * 0.9f;
	lit.Position = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
	lit.Range = 100.0f;
	lit.Attenuation0 = 0.0f;
	lit.Attenuation1 = 0.9f;
	lit.Attenuation2 = 0.0f;
	if (false == g_light.create(Device, lit))
		return false;

	// Position and aim the camera.
	D3DXVECTOR3 pos(0.0f, 14.0f, -8.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 2.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_mView, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &g_mView);

	// Set the projection matrix.
	D3DXMatrixPerspectiveFovLH(&g_mProj, D3DX_PI / 4,
		(float)Width / (float)Height, 1.0f, 100.0f);
	Device->SetTransform(D3DTS_PROJECTION, &g_mProj);

	// Set render states.
	Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	g_light.setLight(Device, g_mWorld);
	return true;
}

void Cleanup(void)
{
	g_legoPlane.destroy();
	for (int i = 0; i < 4; i++) {
		g_legowall[i].destroy();
	}
	destroyAllLegoBlock();
	g_light.destroy();
}


// timeDelta represents the time between the current image frame and the last image frame.
// the distance of moving balls should be "velocity * timeDelta"
bool Display(float timeDelta)
{
	int i = 0;
	int j = 0;


	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
		Device->BeginScene();

		// update the position of each ball. during update, check whether each ball hit by walls.
		for (i = 0; i < 3; i++) {
			g_shotBall.ballUpdate(timeDelta);
			if (g_legowall[i].hitBy(g_shotBall)) {
				g_shotBall.setPower(0, 0);
				g_shotBall.setCenter(g_holderBall.getCenter().x, (float)M_RADIUS, -3.88f);
				isShot = false;
			}
		}

		g_holderBall.hitBy(g_shotBall, isShot);

		// check whether any two balls hit together and update the direction of balls
		for (i = 0; i < 54; i++) {
			if (!g_sphere[i].isNull()) g_sphere[i].hitBy(g_shotBall);
			
		}
		g_holderBall.ballUpdate(timeDelta);
		g_shotBall.ballUpdate(timeDelta);

		// draw plane, walls, and spheres
		g_legoPlane.draw(Device, g_mWorld);
		for (i = 0; i < 3; i++) {
			g_legowall[i].draw(Device, g_mWorld);
			//if (!g_sphere[i].isNull()) g_sphere[i].draw(Device, g_mWorld);
		}
		for (i = 0; i < 54; ++i) {
			if (!g_sphere[i].isNull()) g_sphere[i].draw(Device, g_mWorld);
		}
		g_holderBall.draw(Device, g_mWorld);
		g_shotBall.draw(Device, g_mWorld);
		g_light.draw(Device);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
		Device->SetTexture(0, NULL);
	}
	return true;
}

LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool wire = false;
	static bool isReset = true;
	static int old_x = 0;
	static int old_z = 0;
	static enum { WORLD_MOVE, LIGHT_MOVE, BLOCK_MOVE } move = WORLD_MOVE;

	switch (msg) {
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam) {
		case VK_ESCAPE:
			::DestroyWindow(hwnd);
			break;
		case VK_RETURN:
			if (NULL != Device) {
				wire = !wire;
				Device->SetRenderState(D3DRS_FILLMODE,
					(wire ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
			}
			break;
		case VK_SPACE:
			g_shotBall.setPower(0, 2);
			isShot = true;
			break;

		}
		break;
	}

	case WM_MOUSEMOVE:
	{
		int new_x = LOWORD(lParam);
		int new_z = HIWORD(lParam);
		float dx;
		float dz;

		if (LOWORD(wParam) & MK_LBUTTON) {

			if (isReset) {
				isReset = false;
			}
			else {
				D3DXVECTOR3 vDist;
				D3DXVECTOR3 vTrans;
				D3DXMATRIX mTrans;
				D3DXMATRIX mX;
				D3DXMATRIX mZ;

				/*
				switch (move) {
				case WORLD_MOVE:
					dx = (old_x - new_x) * 0.01f;
					dy = (old_y - new_y) * 0.01f;
					D3DXMatrixRotationY(&mX, dx);
					D3DXMatrixRotationX(&mY, dy);
					g_mWorld = g_mWorld * mX * mY;

					break;
				}
				*/
			}

			old_x = new_x;
			old_z = new_z;

		}
		else {
			isReset = true;

			if (LOWORD(wParam) & MK_RBUTTON) {
				dx = (old_x - new_x);// * 0.01f;
				dz = (old_z - new_z);// * 0.01f;

				D3DXVECTOR3 Coord3d = g_holderBall.getCenter();

				if (Coord3d.x + dx * (-0.007f) <= 2.79f && Coord3d.x + dx * (-0.007f) >= -2.79f) {
					g_holderBall.setCenter(Coord3d.x + dx * (-0.007f), Coord3d.y, Coord3d.z);
					if (!isShot) {	
						g_shotBall.setCenter(Coord3d.x + dx * (-0.007f), Coord3d.y, Coord3d.z + 0.42f);
					}
				}

			}
			old_x = new_x;
			old_z = new_z;

			move = WORLD_MOVE;
		}
		break;
	}
	}

	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	srand(static_cast<unsigned int>(time(NULL)));

	if (!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}