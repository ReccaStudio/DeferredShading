#pragma once

class Object
{
public:
	Object();
	virtual ~Object();

	virtual void Update( float dt ) {}

protected:

};

class Object3D : public Object
{
public:
	Object3D();
	virtual ~Object3D();

	virtual void Update( float dt ) {}

protected:
};


class Light3D : public Object3D
{
public:
	Light3D();
	virtual ~Light3D();

	virtual void Update( float dt ) {}

protected:

};

class MeshObject : public Object3D
{
public:
	MeshObject();
	virtual ~MeshObject();

	virtual void Update( float dt ) {}

protected:

};

class PointLight3D : public Light3D
{
public:
	PointLight3D();
	virtual ~PointLight3D();

	virtual void Update( float dt ) {}

protected:

};

class DirectionLight3D : public Light3D
{
public:
	DirectionLight3D();
	virtual ~DirectionLight3D();

	virtual void Update( float dt ) {}

protected:

};

class SpotLight3D : public Light3D
{
public:
	SpotLight3D();
	virtual ~SpotLight3D();

	virtual void Update( float dt ) {}

protected:

};