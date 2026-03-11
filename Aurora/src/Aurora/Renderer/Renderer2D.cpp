#include "arpch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "VertexArray.h"
#include "RenderCommand.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Aurora
{
    // 四边形顶点结构体
    // 包含渲染一个四边形所需的全部顶点数据
    struct QuadVertex
    {
        glm::vec3 Position;     // 顶点位置
        glm::vec4 Color;        // 顶点颜色
        glm::vec2 TexCoord;     // 纹理坐标
        float TexIndex;         // 纹理槽索引
        float TilingFactor;     // 纹理平铺因子
    };

    // 渲染器2D的静态数据
    // 存储所有渲染状态和资源
    struct Renderer2DData
    {
        // 渲染限制常量
        static const uint32_t MaxQuads = 20000;         // 单次批处理最大四边形数量
        static const uint32_t MaxVertices = MaxQuads * 4; // 最大顶点数（每个四边形4个顶点）
        static const uint32_t MaxIndices = MaxQuads * 6;  // 最大索引数（每个四边形6个索引，2个三角形）
        static const uint32_t MaxTextureSlots = 32;       // 最大纹理槽数量（GPU纹理采样器限制）

        // 渲染资源
        Ref<VertexArray> QuadVertexArray;     // 顶点数组对象
        Ref<VertexBuffer> QuadVertexBuffer;   // 顶点缓冲区
        Ref<Shader> TextureShader;            // 纹理着色器
        Ref<Texture2D> WhiteTexture;          // 1x1白色纹理（用于纯色四边形）

        // 渲染状态
        uint32_t QuadIndexCount = 0;           // 当前批次中已提交的索引数量
        QuadVertex* QuadVertexBufferBase = nullptr;  // 顶点缓冲区起始地址
        QuadVertex* QuadVertexBufferPtr = nullptr;    // 当前顶点写入位置

        // 纹理管理
        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;  // 纹理槽数组
        uint32_t TextureSlotIndex = 1;                             // 下一个可用的纹理槽索引（0为白色纹理）

        // 几何数据
        glm::vec4 QuadVertexPositions[4];    // 标准四边形顶点位置（中心在原点，边长为1）

        // 统计信息
        Renderer2D::Statistics Stats;         // 渲染统计

        // 常用常量
        static constexpr glm::vec4 WhiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };  // 白色
        static constexpr glm::vec2 DefaultTexCoords[4] = {   // 默认纹理坐标
            { 0.0f, 0.0f },  // 左下角
            { 1.0f, 0.0f },  // 右下角
            { 1.0f, 1.0f },  // 右上角
            { 0.0f, 1.0f }   // 左上角
        };
    };

    // 静态数据实例
    static Renderer2DData s_Data;

    // 辅助函数：设置单个顶点数据
    // 参数：transform - 模型变换矩阵
    //      color - 顶点颜色
    //      texCoord - 纹理坐标
    //      texIndex - 纹理索引
    //      tilingFactor - 纹理平铺因子
    //      vertexIndex - 顶点索引（0-3，对应四边形的四个顶点）
    static void SetQuadVertexData(const glm::mat4& transform, const glm::vec4& color,
        const glm::vec2& texCoord, float texIndex, float tilingFactor, uint32_t vertexIndex)
    {
        // 应用变换矩阵到顶点位置
        s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[vertexIndex];
        s_Data.QuadVertexBufferPtr->Color = color;            // 设置顶点颜色
        s_Data.QuadVertexBufferPtr->TexCoord = texCoord;      // 设置纹理坐标
        s_Data.QuadVertexBufferPtr->TexIndex = texIndex;       // 设置纹理槽索引
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor; // 设置纹理平铺因子
        s_Data.QuadVertexBufferPtr++;                          // 移动到下一个顶点位置
    }

    // 辅助函数：查找或添加纹理索引
    // 参数：texture - 要查找的纹理
    // 返回值：纹理在纹理槽中的索引（float类型用于shader传输）
    static float GetTextureIndex(const Ref<Texture2D>& texture)
    {
        // 在已绑定的纹理槽中查找
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
        {
            // 比较纹理对象是否相同
            if (*s_Data.TextureSlots[i] == *texture)
            {
                return static_cast<float>(i);  // 找到，返回已有索引
            }
        }

        // 纹理槽已满，使用白色纹理作为后备
        if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
            return 0.0f; // 使用白色纹理的索引（0）

        // 纹理未绑定，添加到下一个可用槽
        float textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex++;  // 更新下一个可用槽索引

        return textureIndex;
    }

    // 初始化渲染器
    // 创建和管理所有渲染资源
    void Renderer2D::Init()
    {
        // 创建顶点数组对象
        s_Data.QuadVertexArray = VertexArray::Create();

        // 创建顶点缓冲区
        s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

        // 设置顶点布局
        s_Data.QuadVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },       // 位置：3个float
            { ShaderDataType::Float4, "a_Color" },          // 颜色：4个float
            { ShaderDataType::Float2, "a_TexCoord" },       // 纹理坐标：2个float
            { ShaderDataType::Float, "a_TextureIndex" },    // 纹理索引：1个float
            { ShaderDataType::Float, "a_TilingFactor" }     // 平铺因子：1个float
            });

        // 将顶点缓冲区添加到顶点数组
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        // 分配顶点缓冲区内存
        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

        // 生成索引数据
        std::vector<uint32_t> quadIndices;
        quadIndices.resize(s_Data.MaxIndices);

        uint32_t offset = 0;
        // 每个四边形由2个三角形组成，6个索引
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
        {
            // 第一个三角形：顶点0,1,2
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            // 第二个三角形：顶点2,3,0
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;  // 每个四边形4个顶点
        }

        // 创建索引缓冲区
        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices.data(), s_Data.MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(quadIB);

        // 创建白色纹理（1x1像素，纯白色）
        s_Data.WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;  // ARGB格式，纯白色
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        // 准备纹理采样器数组
        int32_t samplers[Renderer2DData::MaxTextureSlots];
        for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
        {
            samplers[i] = static_cast<int32_t>(i);  // 采样器ID对应纹理槽索引
        }

        // 创建并设置纹理着色器
        s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetIntArray("u_Textures", samplers, Renderer2DData::MaxTextureSlots);

        // 白色纹理放在纹理槽0
        s_Data.TextureSlots[0] = s_Data.WhiteTexture;

        // 定义标准四边形顶点位置（以原点为中心，边长1的正方形）
        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };  // 左下
        s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };   // 右下
        s_Data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };     // 右上
        s_Data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };    // 左上
    }

    // 关闭渲染器，清理资源
    void Renderer2D::Shutdown()
    {
        // 释放顶点缓冲区内存
        delete[] s_Data.QuadVertexBufferBase;
        s_Data.QuadVertexBufferBase = nullptr;
        s_Data.QuadVertexBufferPtr = nullptr;
    }

    

    // 开始场景渲染
    // 参数：camera - 正交相机
    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        // 绑定着色器
        s_Data.TextureShader->Bind();
        // 设置视图投影矩阵
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

        // 重置渲染状态
        s_Data.QuadIndexCount = 0;                    // 重置索引计数
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;  // 重置顶点缓冲区写入位置
        s_Data.TextureSlotIndex = 1;                  // 重置纹理槽索引（0是白色纹理）
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4 transfom)
    {
        glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transfom); 
        // 绑定着色器
        s_Data.TextureShader->Bind();
        // 设置视图投影矩阵
        s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

        // 重置渲染状态
        s_Data.QuadIndexCount = 0;                    // 重置索引计数
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;  // 重置顶点缓冲区写入位置
        s_Data.TextureSlotIndex = 1;                  // 重置纹理槽索引（0是白色纹理）
    }
    // 结束场景渲染
    // 将顶点数据上传到GPU并执行绘制
    void Renderer2D::EndScene()
    {
        // 没有数据需要渲染，直接返回
        if (s_Data.QuadIndexCount == 0)
            return;

        // 计算顶点数据大小
        uint32_t dataSize = static_cast<uint32_t>(
            reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferPtr) -
            reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferBase));

        // 上传顶点数据到GPU
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

        // 执行渲染
        Flush();
    }

    // 执行实际渲染
    void Renderer2D::Flush()
    {
        // 没有数据需要渲染，直接返回
        if (s_Data.QuadIndexCount == 0)
            return;

        // 绑定所有使用的纹理
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
        {
            s_Data.TextureSlots[i]->Bind(i);
        }

        // 执行绘制调用
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

        // 更新统计信息
        s_Data.Stats.DrawCalls++;
    }

    // 刷新并重置渲染器状态
    // 用于在达到批次限制时开始新的批次
    void Renderer2D::FlushAndReset()
    {
        // 提交当前批次
        EndScene();

        // 重置渲染状态
        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
        s_Data.TextureSlotIndex = 1;
    }

    // 通用四边形绘制内部函数
    // 参数：transform - 模型变换矩阵
    //      color - 颜色
    //      texCoords - 4个顶点的纹理坐标数组
    //      textureIndex - 纹理索引
    //      tilingFactor - 纹理平铺因子
    static void DrawQuadInternal(const glm::mat4& transform, const glm::vec4& color,
        const glm::vec2 texCoords[4], float textureIndex, float tilingFactor)
    {
        // 检查是否达到批次限制
        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            Renderer2D::FlushAndReset();

        // 设置四个顶点数据
        for (uint32_t i = 0; i < 4; i++)
        {
            SetQuadVertexData(transform, color, texCoords[i], textureIndex, tilingFactor, i);
        }

        // 更新索引计数（每个四边形6个索引）
        s_Data.QuadIndexCount += 6;
        // 更新统计信息
        s_Data.Stats.QuadCount++;
    }

    // 带纹理的通用四边形绘制内部函数
    // 参数：transform - 模型变换矩阵
    //      texture - 纹理
    //      texCoords - 4个顶点的纹理坐标数组
    //      tilingFactor - 纹理平铺因子
    //      tintColor - 着色颜色
    static void DrawTexturedQuadInternal(const glm::mat4& transform, const Ref<Texture2D>& texture,
        const glm::vec2 texCoords[4], float tilingFactor, const glm::vec4& tintColor)
    {
        // 检查是否达到批次限制
        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            Renderer2D::FlushAndReset();

        // 获取纹理索引
        float textureIndex = GetTextureIndex(texture);

        // 绘制四边形
        DrawQuadInternal(transform, tintColor, texCoords, textureIndex, tilingFactor);
    }

    //----------------------------------------------------------------
    // 纯色四边形绘制函数（2D位置）
    //----------------------------------------------------------------
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        // 转换为3D位置调用
        DrawQuad({ position.x, position.y, 0.0f }, size, color);
    }

    // 纯色四边形绘制函数（3D位置）
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        // 计算变换矩阵：平移 + 缩放
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        // 调用内部绘制函数（使用白色纹理索引0）
        DrawQuadInternal(transform, color, Renderer2DData::DefaultTexCoords, 0.0f, 1.0f);
    }

    //----------------------------------------------------------------
    // 纹理四边形绘制函数（2D位置）
    //----------------------------------------------------------------
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
        const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        // 转换为3D位置调用
        DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
    }

    // 纹理四边形绘制函数（3D位置）
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
        const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        // 计算变换矩阵
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        // 调用带纹理的内部绘制函数
        DrawTexturedQuadInternal(transform, texture, Renderer2DData::DefaultTexCoords, tilingFactor, tintColor);
    }

    //----------------------------------------------------------------
    // 子纹理四边形绘制函数（2D位置）
    //----------------------------------------------------------------
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
        const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
    {
        // 转换为3D位置调用
        DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, tilingFactor, tintColor);
    }

    // 子纹理四边形绘制函数（3D位置）
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
        const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
    {
        // 计算变换矩阵
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        // 调用带纹理的内部绘制函数，使用子纹理的纹理坐标
        DrawTexturedQuadInternal(transform, subtexture->GetTexture(),
            subtexture->GetTexCoords(), tilingFactor, tintColor);
    }

    //----------------------------------------------------------------
    // 使用变换矩阵的纯色四边形绘制函数
    //----------------------------------------------------------------
    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
    {
        DrawQuadInternal(transform, color, Renderer2DData::DefaultTexCoords, 0.0f, 1.0f);
    }

    // 使用变换矩阵的纹理四边形绘制函数
    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture,
        float tilingFactor, const glm::vec4& tintColor)
    {
        DrawTexturedQuadInternal(transform, texture, Renderer2DData::DefaultTexCoords, tilingFactor, tintColor);
    }

    //----------------------------------------------------------------
    // 旋转纯色四边形绘制函数（2D位置）
    //----------------------------------------------------------------
    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size,
        float rotation, const glm::vec4& color)
    {
        // 转换为3D位置调用
        DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
    }

    // 旋转纯色四边形绘制函数（3D位置）
    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size,
        float rotation, const glm::vec4& color)
    {
        // 计算变换矩阵：平移 + 旋转 + 缩放
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })  // 绕Z轴旋转
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        // 调用内部绘制函数
        DrawQuadInternal(transform, color, Renderer2DData::DefaultTexCoords, 0.0f, 1.0f);
    }

    //----------------------------------------------------------------
    // 旋转纹理四边形绘制函数（2D位置）
    //----------------------------------------------------------------
    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
        const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        // 转换为3D位置调用
        DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
    }

    // 旋转纹理四边形绘制函数（3D位置）
    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
        const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        // 计算变换矩阵：平移 + 旋转 + 缩放
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        // 调用带纹理的内部绘制函数
        DrawTexturedQuadInternal(transform, texture, Renderer2DData::DefaultTexCoords, tilingFactor, tintColor);
    }

    //----------------------------------------------------------------
    // 旋转子纹理四边形绘制函数（2D位置）
    //----------------------------------------------------------------
    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
        const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
    {
        // 转换为3D位置调用
        DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture, tilingFactor, tintColor);
    }

    // 旋转子纹理四边形绘制函数（3D位置）
    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
        const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
    {
        // 计算变换矩阵：平移 + 旋转 + 缩放
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        // 调用带纹理的内部绘制函数，使用子纹理的纹理坐标
        DrawTexturedQuadInternal(transform, subtexture->GetTexture(),
            subtexture->GetTexCoords(), tilingFactor, tintColor);
    }

    //----------------------------------------------------------------
    // 统计信息相关函数
    //----------------------------------------------------------------

    // 重置渲染统计
    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    // 获取当前渲染统计
    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return s_Data.Stats;
    }
}