#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type;
    while (getline(stream, line))
    {
        if(line.find("#shader") != std::string::npos)
        {
            if(line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if(line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

// 编译给定类型的着色器
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); // 创建指定类型的着色器
    const char* src = source.c_str(); // 将源代码转换为C风格字符串
    glShaderSource(id, 1, &src, nullptr); // 设置着色器源代码
    glCompileShader(id); // 编译着色器

    // 检查编译是否出错
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) // 如果编译失败
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); // 获取错误信息的长度
        char* message = (char*)alloca(length * sizeof(char)); // 为错误信息分配内存
        glGetShaderInfoLog(id, length, &length, message); // 获取错误信息
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id); // 删除失败的着色器
        return 0;
    }

    return id; // 如果编译成功，返回着色器ID
}

// 从顶点着色器和片段着色器创建着色器程序
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); // 创建新的着色器程序
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); // 编译顶点着色器
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader); // 编译片段着色器

    glAttachShader(program, vs); // 将顶点着色器附加到程序
    glAttachShader(program, fs); // 将片段着色器附加到程序
    glLinkProgram(program); // 链接着色器形成完整的程序
    glValidateProgram(program); // 验证程序是否有效

    glDeleteShader(vs); // 删除顶点着色器（链接后不再需要）
    glDeleteShader(fs); // 删除片段着色器

    return program; // 返回着色器程序ID
}

int main(void)
{
    GLFWwindow* window;

    // 初始化GLFW库
    if (!glfwInit())
        return -1;

    // 创建一个窗口模式的窗口和它的OpenGL上下文
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if (!window) // 检查窗口是否创建成功
    {
        glfwTerminate(); // 如果窗口创建失败，终止GLFW
        return -1;
    }

    // 使窗口的OpenGL上下文为当前
    glfwMakeContextCurrent(window);

    // 初始化GLEW（管理OpenGL扩展）
    if (glewInit() != GLEW_OK)
        std::cout << "Error initializing GLEW!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl; // 输出OpenGL版本

    // 定义三角形的顶点位置
    float position[] = {
        -0.5f, -0.5f, //0
         0.5f, -0.5f, //1
         0.5f,  0.5f, //2
        -0.5f,  0.5f, //3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // 生成并绑定缓冲区来存储顶点数据
    unsigned int buffer;
    glGenBuffers(1, &buffer); // 生成缓冲区ID
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // 将缓冲区绑定为活动数组缓冲
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), position, GL_STATIC_DRAW); // 将顶点数据上传到缓冲区

    // 启用位置为0的顶点属性
    glEnableVertexAttribArray(0);
    // 定义顶点属性的格式和来源
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // 生成并绑定缓冲区来存储顶点数据
    unsigned int ibo;
    glGenBuffers(1, &ibo); // 生成缓冲区ID
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // 将缓冲区绑定为活动数组缓冲
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW); // 将顶点数据上传到缓冲区

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

    // 创建并激活着色器程序
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲区

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window); // 交换前后缓冲区
        glfwPollEvents(); // 轮询并处理事件
    }

    glDeleteProgram(shader);

    glfwTerminate(); // 清理并退出
    return 0;
}
