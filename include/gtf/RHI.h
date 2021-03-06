//
//  RHI.h
//  MeshPainter
//
//  Created by David Gallardo on 17/10/15.
//  Copyright (c) 2015 David Gallardo. All rights reserved.
//

#pragma once

#include <stdlib.h>
#include <string>
#include <vector>

namespace gtf 
{
enum ERHIShaderType
{
    RHI_VERTEX_SHADER = 0,
    RHI_FRAGMENT_SHADER = 1
};

enum ERHITexTarget
{
    RHI_TEXTURE_1D = 0,
    RHI_TEXTURE_2D = 1
};

enum ERHITexInternalFormat
{
    RHI_RGB8 = 0,
    RHI_RGBA8,
    RHI_DEPTH32,
    RHI_RGBA32F,
    RHI_RGB32F,
    RHI_RGBA16F,
    RHI_RGB16F,
    RHI_RG16F,
    RHI_R16F,
    RHI_R8,
    RHI_DEPTH8
};

enum ERHITexFormat
{
    RHI_DEPTH_COMPONENT = 0,
    RHI_DEPTH_STENCIL = 1,
    RHI_RED = 2,
    RHI_RG = 3,
    RHI_RGB = 4,
    RHI_RGBA = 5,
    RHI_BGR,
    RHI_BGRA
};

enum ERHIValueType
{
    RHI_FLOAT = 0,
    RHI_INT = 1,
    RHI_UNSIGNED_BYTE = 2
};

enum ERHIBlendFnc
{
    RHI_ZERO = 0,
    RHI_ONE = 1,
    RHI_SRC_COLOR = 2,
    RHI_ONE_MINUS_SRC_COLOR = 3,
    RHI_DST_COLOR = 4,
    RHI_ONE_MINUS_DST_COLOR = 5,
    RHI_SRC_ALPHA = 6,
    RHI_ONE_MINUS_SRC_ALPHA = 7,
    RHI_DST_ALPHA = 8,
    RHI_ONE_MINUS_DST_ALPHA = 9,
    RHI_CONSTANT_COLOR = 10,
    RHI_ONE_MINUS_CONSTANT_COLOR = 11,
    RHI_CONSTANT_ALPHA = 12,
    RHI_ONE_MINUS_CONSTANT_ALPHA = 13,
    RHI_SRC_ALPHA_SATURATE = 14
};

enum ERHIBlendEquation
{
    RHI_FUNC_ADD = 0,
    RHI_FUNC_SUBTRACT = 1,
    RHI_FUNC_REVERSE_SUBTRACT = 2,
    RHI_MIN = 3,
    RHI_MAX = 4
};

enum ERHITexParamName
{
    RHI_TEXTURE_MIN_FILTER = 5,
    RHI_TEXTURE_MAG_FILTER = 6,
    RHI_TEXTURE_WRAP_S = 14,
    RHI_TEXTURE_WRAP_T = 15,
};

enum ERHITexParamValues
{
    RHI_NEAREST = 1,
    RHI_LINEAR = 2,
    RHI_NEAREST_MIPMAP_NEAREST = 3,
    RHI_LINEAR_MIPMAP_NEAREST = 4,
    RHI_NEAREST_MIPMAP_LINEAR = 5,
    RHI_LINEAR_MIPMAP_LINEAR = 6,
    RHI_CLAMP_TO_EDGE = 7,
    RHI_MIRRORED_REPEAT = 8,
    RHI_REPEAT = 9
};

enum ERHICompareFunc
{
    RHI_LEQUAL = 0,
    RHI_GEQUAL = 1,
    RHI_LESS = 2,
    RHI_GREATER = 3,
    RHI_EQUAL = 4,
    RHI_NOTEQUAL = 5,
    RHI_ALWAYS = 6,
    RHI_NEVER = 7
};

/*
struct RHIVertex
{
    float x = 0, y = 0, z = 0;          //position
    float nx = 0, ny = 0, nz = 0;       //normal
    float tcx = 0, tcy = 0;             //texcoord
    float tnx = 0, tny = 0, tnz = 0;    //tangent
    float bnx = 0, bny = 0, bnz = 0;    //binormal
};

struct RHIUVRegionVertex
{
	float x, y;
	float faceId, regionId;
};
*/

struct RHIVertexAttribute
{
	size_t offset;
	unsigned int components;
	ERHIValueType type;
	bool normalized;
	

	RHIVertexAttribute(size_t _o, unsigned int _c, ERHIValueType _t, bool _n)
		: offset(_o)
		, components(_c)
		, type(_t)
		, normalized(_n) {};
};

struct RHITextureInfo
{
    ERHITexInternalFormat internalFormat;
    unsigned int bufferW;
    unsigned int bufferH;
    ERHITexFormat format;
    ERHIValueType type;
    unsigned int size;
};

struct RHITexParam
{
    ERHITexParamName name;
    ERHITexParamValues value;
};

struct RHITexturePreset : public RHITextureInfo
{
    const void* buffer = nullptr;
    RHITexParam texParams[4];
    unsigned int numParams = 0;
};

class RHITexture2D
{
public:
    virtual ~RHITexture2D(){};
    void setup(RHITextureInfo const & info, const void* data = nullptr);
    void setup(RHITexturePreset const & preset);
    virtual void setup(ERHITexInternalFormat inFormat,
                       unsigned int bufferW,
                       unsigned int bufferH,
                       ERHITexFormat format,
                       ERHIValueType type,
                       const void* buffer = nullptr,
                       const RHITexParam* texParams = nullptr,
                       unsigned int numParams = 0) = 0;
    virtual void bindAt(unsigned int texUnit) = 0;
    virtual void unbind() = 0;
    virtual unsigned int getName() const = 0;
    virtual void getInfo(RHITextureInfo & info) const = 0;
    virtual void copyToBuffer(void* buffer, ERHITexFormat format, ERHIValueType valType) = 0;
    virtual void copyFromBuffer(void* buffer) = 0;
    virtual void clearWithColor(float r, float g = 0.0f, float b = 0.0f, float a = 0.0f) = 0;
    virtual class RHITexture2D * cloneTexture(bool withData = false) = 0;
    virtual void flipY() = 0;
};


class RHICubeMap
{
public:
    virtual ~RHICubeMap(){};
    virtual void setup(class RHITexture2D ** faces, int lodLvl) = 0;
    virtual void bindAt(unsigned int texUnit) = 0;
    virtual void unbind() = 0;
};

class RHIFrameBuffer
{
public:
    virtual ~RHIFrameBuffer(){};
    virtual void setup(RHITexture2D ** attachments,
                       unsigned int numAttachments = 1,
                       RHITexture2D * depthAttach = nullptr) = 0;
    virtual void bindForDrawing() = 0;
    virtual void bindForReading() = 0;
    virtual void unbindForDrawing() = 0;
    virtual void unbindForReading() = 0;
    virtual unsigned int getName() const = 0;
    virtual void getPixelAt(int x, int y, unsigned int attachIndex, void* buffer) = 0;
    virtual void copyAttachmentToTexture(unsigned int attachment, RHITexture2D * outputTex) = 0;
    virtual void clearAttachment(unsigned int attachIndex,
                                 float r = 0.0f,
                                 float g = 0.0f,
                                 float b = 0.0f,
                                 float a = 0.0f) = 0;
    
    virtual void clearDepthBuffer(float d = 1.0f) = 0;
};

class RHIShaderSource
{
public:
    virtual ~RHIShaderSource(){};
    virtual void compileSource(const char * sourceStr, ERHIShaderType type) = 0;
    virtual unsigned int getName() const = 0;
    virtual ERHIShaderType getType() const = 0;
};

class RHIShaderProgram
{
public:
    virtual ~RHIShaderProgram(){};
    virtual void link(RHIShaderSource** sources, size_t sourceCount) = 0;
    virtual void active() = 0;
        
    virtual void setUniform1ui(const char * ref, unsigned int value) = 0;
    virtual void setUniform1i(const char * ref, int value) = 0;
    virtual void setUniform1f(const char * ref, float v1) = 0;
    virtual void setUniform2f(const char * ref, float v1, float v2) = 0;
    virtual void setUniform4f(const char * ref, float v1, float v2, float v3, float v4) = 0;
    virtual void setUniform4x4m(const char * ref, float* mat4x4) = 0;
    virtual void setUniform3fv(const char * ref, unsigned int numElems, float* vec3array) = 0;
};


class RHIVertexAttributeList
{
public:
	RHIVertexAttributeList(size_t structSize);
	void addAttribute(RHIVertexAttribute & attr);
	void addAttribute(size_t offset, unsigned int components, ERHIValueType type = ERHIValueType::RHI_FLOAT, bool normalized = false);
	std::vector<RHIVertexAttribute> const & getIterableList() const;
	unsigned int getStructSize() const;
private:
	std::vector<RHIVertexAttribute> m_attrList;
	unsigned int m_totalStructSize{ 0 };
};

class RHIVAO
{
public:
    virtual ~RHIVAO(){};
    virtual void render() = 0;
	virtual void setup(RHIVertexAttributeList const & list, void* vertexList, unsigned int vertexCount) = 0;
};




class RHI
{
public:
    static RHI* instance;
    
    virtual int init() = 0;
    
    //render api methods
    virtual void setClearColor(float r, float g, float b, float a) = 0;
    virtual void clearColorAndDepthBuffers() = 0;
    virtual void viewport(int x, int y, int w, int h) = 0;
    virtual void setDepthTest(bool enabled) = 0;
    virtual void setBackfaceCulling(bool enabled) = 0;
    virtual void emitVertexCommand(unsigned int numVertex) = 0;
    virtual void setBlend(bool enabled) = 0;
    virtual void setBlendFnc(ERHIBlendFnc colorSrcFactor = ERHIBlendFnc::RHI_SRC_ALPHA,
                             ERHIBlendFnc colorDstFactor = ERHIBlendFnc::RHI_ONE_MINUS_SRC_ALPHA,
                             ERHIBlendFnc alphaSrcFactor = ERHIBlendFnc::RHI_SRC_ALPHA,
                             ERHIBlendFnc alphaDstFactor = ERHIBlendFnc::RHI_ONE_MINUS_SRC_ALPHA) = 0;
    virtual void setBlendEquation(ERHIBlendEquation colorEq = ERHIBlendEquation::RHI_FUNC_ADD,
                                  ERHIBlendEquation alphaEq = ERHIBlendEquation::RHI_FUNC_ADD) = 0;
    
    //factory methods
    virtual RHIVAO * createVertexBufferObject() = 0;
    virtual RHIShaderProgram * createProgram() = 0;
    virtual RHIShaderSource * createSource() = 0;
    virtual RHITexture2D * createTexture() = 0;
    virtual RHICubeMap * createCubeMap() = 0;
    virtual RHIFrameBuffer * createFramebuffer() = 0;
    
    //shader utils
	//add a new shader header source
	virtual void addShaderHeader(const char * headerName, const char * headerContent) = 0;

	//replaces all header included in shader source by it's content
	virtual std::string solveShaderHeaders(const char * shaderSource) = 0;
};

extern RHI* GRHI;

} //namespace gtf

