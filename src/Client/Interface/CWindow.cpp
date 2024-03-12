// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

glm::mat4 CWindow::m_VP;
float CWindow::m_Step = 0.f;

CWindow::CWindow()
{
    g_Window = NULL;
    m_ProgramId = 0;
}

CWindow::~CWindow()
{
}

const glm::mat4& CWindow::GetVP() { return m_VP; }

bool CWindow::Initialize()
{
    std::cout << "Iniciando glfw...\n";

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    g_Window = glfwCreateWindow(g_WindowMaxX, g_WindowMaxY, "Space Invaders", NULL, NULL);
    if (!g_Window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(g_Window);
    glfwSwapInterval(0); // Disable vsync

    ::CEvent::Initialize();

    std::cout << "Iniciando glew...\n";

    /* Initialize glew and OpenGL functions */
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;

        glfwTerminate();
        return false;
    }

    std::cout << "Iniciando imgui...\n";

    /* Initialize ImGui */
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
    ImGui_ImplOpenGL3_Init();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    /* Compile and Link Shaders */
    std::cout << "Compilando o Vertex Shader...\n";
    GLuint vShaderId = CompileShader(CUtil::m_VertexShader, GL_VERTEX_SHADER);

    std::cout << "Compilando o Fragment Shader...\n";
    GLuint fShaderId = CompileShader(CUtil::m_FragmentShader, GL_FRAGMENT_SHADER);

    m_ProgramId = LinkProgram(vShaderId, fShaderId);

    // Use Shaders.
    glUseProgram(m_ProgramId);

    std::cout << "Carregando os modelos...\n";

    m_Game.Initialize();

    // Configure the Lines.
    glLineWidth(2.f);
    glEnable(GL_LINE_SMOOTH);

    std::cout << "Iniciando...\n";
	return true;
}

void CWindow::Cleanup()
{
    /* Cleanup ImGui */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    /* Cleanup GLFW */
    glfwDestroyWindow(g_Window);
    glfwTerminate();
}

bool CWindow::Render()
{
    // Clear OpenGl frame.
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set Transforms.
    glm::mat4 projection = glm::perspective(glm::radians(m_Camera.m_Zoom), (float)g_WindowMaxY / (float)g_WindowMaxX, 0.1f, 100.0f);
    glm::mat4 view = m_Camera.GetViewMatrix();

    m_VP = (projection * view);
    glUniformMatrix4fv(glGetUniformLocation(m_ProgramId, "u_vp"), 1, GL_FALSE, glm::value_ptr(m_VP));
    
    // Draw objects.
    for (unsigned int i = 0; i < MAX_OBJECT; ++i)
    {
        CModel* m = CModel::GetModel(i);
        if (m)
            m->Draw(m_ProgramId, m_VP);
    }

    // Start the Dear ImGui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static int Selecteditem = 0;

    float newStep = m_Step;
    int newSelectItem = Selecteditem;
    float newMass = CModel::GetModelFromId(Selecteditem + 1)->m_Mass;

    // Create ImGui Sliders.
    ImGui::Begin("nbody");
        ImGui::Text("Movimentacao: W/A/S/D");

        ImGui::NewLine();
        if (ImGui::Button("Reiniciar"))
            CGame::m_Rpc->call("reload");

        ImGui::NewLine();
        ImGui::Text("Step:");
        ImGui::SliderFloat("step", &newStep, 0.01f, 0.09f);

        ImGui::NewLine();
        ImGui::Combo("P", &Selecteditem, CGame::m_BodyName, CGame::m_BodyNameSize);
        ImGui::SliderFloat("mass", &newMass, 1.f, 100.f);

        ImGui::NewLine();
        ImGui::Text("Desenvolvido por:");
        ImGui::Text("Junior Zanol");
        ImGui::Text("jrzanol.92@gmail.com");

        ImGui::NewLine();
        ImGui::Text("FPS: %u", g_FPS);
    ImGui::End();

    if (newStep != m_Step)
    {
        m_Step = newStep;

        CGame::m_Rpc->call("setStep", newStep);
    }

    if (newSelectItem == Selecteditem && newMass != CModel::GetModelFromId(Selecteditem + 1)->m_Mass)
    {
        CModel::GetModelFromId(Selecteditem + 1)->m_Mass = newMass;

        CGame::m_Rpc->call("setMass", Selecteditem + 1, newMass);
    }

    // Rendering the ImGui.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers.
    glfwSwapBuffers(g_Window);

    // Poll for and process events.
    glfwPollEvents();

    // Process Input events.
    for (const auto& it : *g_EventList)
        it->ProcessInput(g_Window);

    return glfwWindowShouldClose(g_Window);
}

GLuint CWindow::CompileShader(const char* shaderCode, GLenum type)
{
    GLuint shaderId = glCreateShader(type);

    if (shaderId == 0) { // Error: Cannot create shader object
        std::cout << "Error creating shaders";
        return 0;
    }

    // Attach source code to this object
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId); // compile the shader object

    GLint compileStatus;

    // check for compilation status
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus) { // If compilation was not successful
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* cMessage = new char[length];

        // Get additional information
        glGetShaderInfoLog(shaderId, length, &length, cMessage);
        std::cout << "Cannot Compile Shader: " << cMessage;
        delete[] cMessage;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

GLuint CWindow::LinkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint programId = glCreateProgram(); // create a program

    if (programId == 0) {
        std::cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Create executable of this program
    glLinkProgram(programId);

    GLint linkStatus;

    // Get the link status for this program
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus) { // If the linking failed
        std::cout << "Error Linking program";
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);
        glDeleteProgram(programId);

        return 0;
    }

    return programId;
}

